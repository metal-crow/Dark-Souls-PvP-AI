#include "HelperUtil.h"
#include <stdio.h>//printf
#include "Settings.h"

#pragma warning( disable: 4244 )//ignore dataloss conversion from double to long

//store camera settings
typedef struct {
	ullong cam_x_addr;
	float cam_x;
	ullong cam_y_addr;
	float cam_y;
	ullong rot_x_addr;
	float rot_x;
	ullong rot_y_addr;
	float rot_y;
} CameraSett;

static CameraSett * camera;

//address data to read camera settings
static ullong camera_base = 0x00F5CDD4;
static const int camera_y_offsets[] = { 0x174,0x4D4,0x144,0x320,0xF8 };
static const int camera_x_offsets[] = { 0x174, 0x4D4, 0x144, 0x320, 0x90 };
static const int camera_y_rot_offsets[] = { 0x174, 0x4D4, 0x144, 0x320, 0x150 };
static const int camera_x_rot_offsets[] = { 0x174, 0x4D4, 0x144, 0x320, 0x144 };
static const int camera_offsets_length = 5;

//get straight line distance between player and enemy
float distance(Character * Player, Character * Phantom){
    double delta_x = fabsf(fabsf(Player->loc_x) - fabsf(Phantom->loc_x));
    double delta_y = fabsf(fabsf(Player->loc_y) - fabsf(Phantom->loc_y));
    return hypot(delta_x, delta_y);
}

//the absolute value of the angle the opponent is off from straight ahead (returns radians, only used as neural net input)
//TODO this only works from front 180, after which it mirrors. THIS TREATS FRONT SAME AS BACK. BAD.
float angleDeltaFromFront(Character * Player, Character * Phantom){
    double delta_x = fabsf(fabsf(Player->loc_x) - fabsf(Phantom->loc_x));
    double delta_y = fabsf(fabsf(Player->loc_y) - fabsf(Phantom->loc_y));

    //if its closer to either 90 or 270 by 45, its x direction facing
    if (((Player->rotation > 45) && (Player->rotation < 135)) || ((Player->rotation > 225) && (Player->rotation < 315))){
        return atan(delta_y / delta_x);
    } else{
        return atan(delta_x / delta_y);
    }
}

bool AnglesWithinRange(float ang1, float ang2, float range){
    float ang1Max = ang1 + range;
    ang1Max = ang1Max <= 360 ? ang1Max : ang1Max - 360;
    float ang1Min = ang1 - range;
    ang1Min = ang1Min >= 0 ? ang1Min : ang1Min + 360;

    //dont have to worry about 0 problem
    if (ang1Min < ang1Max){
        return ang1Min <= ang2 && ang2 <= ang1Max;
    } else{
        //split into two sides across the 0 mark, check if player in either
        return (ang1Min <= ang2 && ang2 <= 360) || (0 <= ang2 && ang2 <= ang1Max);
    }
}

//since stamina isnt sent over wire estimate the enemy's from last_animation_types_enemy
int StaminaEstimationEnemy(){
    int staminaEstimate = 192;//assume their max stamina is max

    for (int i = last_animation_types_enemy_LENGTH - 1; i >= 0; i--){
        //backsteps. these have diff stamina drain from other rolls
        if (last_animation_types_enemy[i] == Backstep_1H || last_animation_types_enemy[i] == Backstep_2H){
            staminaEstimate -= 19;
        }
        else if (isDodgeAnimation(last_animation_types_enemy[i])){
            staminaEstimate -= 28;
        }
        else if (isAttackAnimation(last_animation_types_enemy[i])){
            //assuming they havent switched weapons during this time, use their right weapon and the attack type to get the stamina drain
            staminaEstimate -= StaminaDrainForAttack(Enemy.r_weapon_id, Enemy.animationType_id);
        }
        //bug: this includes running, which drains stamina
        else if (last_animation_types_enemy[i] == Nothing || last_animation_types_enemy[i] == Shield_Held_Up || last_animation_types_enemy[i] == Shield_Held_Up_walking){
            staminaEstimate += Enemy.staminaRecoveryRate / 10;
        }

        //cap max and min stam
        if (staminaEstimate > 192){
            staminaEstimate = 192;
        } 
        else if (staminaEstimate < -40){
            staminaEstimate = -40;
        }
    }

    guiPrint("%d,5:Stamina Est:%d", EnemyId, staminaEstimate);
    return staminaEstimate;
}

//handles rollover from 360 to 0
//player is +-60 degrees relative to the enemy rotation (yes, thats all to it)
#define BackstabDegreeRange 60
static bool InBackstabRange(float enemy, float player){
    float enemypos = enemy + BackstabDegreeRange;
    enemypos = enemypos <= 360 ? enemypos : enemypos - 360;
    float enemyneg = enemy - BackstabDegreeRange;
    enemyneg = enemyneg >= 0 ? enemyneg : enemyneg + 360;

    //dont have to worry about 0 problem
    if (enemyneg < enemypos){
        return enemyneg <= player && player <= enemypos;
    } else{
        //split into two sides across the 0 mark, check if player in either
        return (enemyneg <= player && player <= 360) || (0 <= player && player <= enemypos);
    }
}

//check if player behind enemy, and if they're in +-60 degree bs window
#define BackstabRange 1.5
//the determiner for if behind someone also must include both characters body widths. or it could be a cone, instead of a line. not sure.
#define RealBehindRange 0.1
unsigned char BackstabDetection_CounterClockwise(Character* Player, Character* Enemy, float distance){
    float angle = Enemy->rotation;
    float y_dist = Enemy->loc_y - Player->loc_y;
    float x_dist = Enemy->loc_x - Player->loc_x;

    //if enemy in 1st segmented area
    //each segment is 90 degrees
    if ((angle <= 360 && angle >= 315) || (angle <= 45 && angle >= 0)){
        //player is behind them
        if (y_dist > RealBehindRange){
            //player is in backstab rotation and distance in allowable range
            if (InBackstabRange(angle, Player->rotation) && distance <= BackstabRange){
                return 2;
            }
            return 1;
        }
        return 0;
    } else if (angle <= 315 && angle >= 225){
        if (x_dist < -RealBehindRange){
            if (InBackstabRange(angle, Player->rotation) && distance <= BackstabRange){
                return 2;
            }
            return 1;
        }
        return 0;
    } else if (angle <= 225 && angle >= 135){
        if (y_dist < -RealBehindRange){
            if (InBackstabRange(angle, Player->rotation) && distance <= BackstabRange){
                return 2;
            }
            return 1;
        }
        return 0;
    } else{
        if (x_dist > RealBehindRange){
            if (InBackstabRange(angle, Player->rotation) && distance <= BackstabRange){
                return 2;
            }
            return 1;
        }
        return 0;
    }
}
    
//this tests if safe FROM backstabs and able TO backstab
//determines the state of a backstab from the perspecitve's view with respect to the target
//0 is no backstab, 1 is behind target, 2 is backstab
unsigned char BackstabDetection(Character* Perspective, Character* Target, float distance){
    return BackstabDetection_CounterClockwise(Perspective, Target, distance);
}

float rotationDifferenceFromSelf(Character * Player, Character * Phantom){
    double delta = fabs((Player->rotation) - (Phantom->rotation));
    return delta;
}

int loadvJoy(UINT iInterface){
	// Get the driver attributes (Vendor ID, Product ID, Version Number)
	if (!vJoyEnabled()){
		printf("vJoy driver not enabled: Failed Getting vJoy attributes.\n");
		return -2;
	} else{
		printf("Vendor: %S\nProduct :%S\nVersion Number:%S\n", GetvJoyManufacturerString(), GetvJoyProductString(), GetvJoySerialNumberString());
	};

	WORD VerDll, VerDrv;
	if (!DriverMatch(&VerDll, &VerDrv)){
		printf("Failed\r\nvJoy Driver (version %04x) does not match vJoyInterface DLL (version %04x)\n", VerDrv, VerDll);
	} else{
		printf("OK - vJoy Driver and vJoyInterface DLL match vJoyInterface DLL (version %04x)\n", VerDrv);
	}

	// Get the state of the requested device
	VjdStat status = GetVJDStatus(iInterface);
	switch (status){
	case VJD_STAT_OWN:
	printf("vJoy Device %d is already owned by this feeder\n", iInterface);
	break;
	case VJD_STAT_FREE:
	printf("vJoy Device %d is free\n", iInterface);
	break;
	case VJD_STAT_BUSY:
	printf("vJoy Device %d is already owned by another feeder\nCannot continue\n", iInterface);
	return -3;
	case VJD_STAT_MISS:
	printf("vJoy Device %d is not installed or disabled\nCannot continue\n", iInterface);
	return -4;
	default:
	printf("vJoy Device %d general error\nCannot continue\n", iInterface);
	return -1;
	};

	// Check which axes are supported
	BOOL AxisX = GetVJDAxisExist(iInterface, HID_USAGE_X);
	BOOL AxisY = GetVJDAxisExist(iInterface, HID_USAGE_Y);
	BOOL AxisZ = GetVJDAxisExist(iInterface, HID_USAGE_Z);
	BOOL AxisRX = GetVJDAxisExist(iInterface, HID_USAGE_RX);
	BOOL AxisRY = GetVJDAxisExist(iInterface, HID_USAGE_RY);
	// Get the number of buttons and POV Hat switchessupported by this vJoy device
	int nButtons = GetVJDButtonNumber(iInterface);
	int ContPovNumber = GetVJDContPovNumber(iInterface);
	int DiscPovNumber = GetVJDDiscPovNumber(iInterface);

	// Print results
	printf("\nvJoy Device %d capabilities:\n", iInterface);
	printf("Numner of buttons\t\t%d\n", nButtons);
	printf("Numner of Continuous POVs\t%d\n", ContPovNumber);
	printf("Numner of Descrete POVs\t\t%d\n", DiscPovNumber);
	printf("Axis X\t\t%s\n", AxisX ? "Yes" : "No");
	printf("Axis Y\t\t%s\n", AxisX ? "Yes" : "No");
	printf("Axis Z\t\t%s\n", AxisX ? "Yes" : "No");
	printf("Axis Rx\t\t%s\n", AxisRX ? "Yes" : "No");
	printf("Axis Ry\t\t%s\n", AxisRY ? "Yes" : "No");

    if (!AxisX || !AxisY || !AxisZ || !AxisRX || !AxisRY || nButtons < 10){
        printf("Invalid config\n");
        return EXIT_FAILURE;
    }

	// Acquire the target
	if ((status == VJD_STAT_OWN) || ((status == VJD_STAT_FREE) && (!AcquireVJD(iInterface)))){
		printf("Failed to acquire vJoy device number %d.\n", iInterface);
		return -1;
	} else{
		printf("Acquired: vJoy device number %d.\n", iInterface);
	}
	return 0;
}

//given player and enemy coordinates, get the angle between the two
double angleFromCoordinates(float player_x, float phantom_x, float player_y, float phantom_y){
#if OolicelMap
	double delta_x = fabsf(player_x) - fabsf(phantom_x);
	double delta_y = fabsf(phantom_y) - fabsf(player_y);
#else
    double delta_x = fabsf(phantom_x) - fabsf(player_x);
    double delta_y = fabsf(player_y) - fabsf(phantom_y);
#endif

	//convert this to 360 degrees
	double angle = (atan2(delta_x, delta_y) + PI) * (180.0 / PI);

#if !OolicelMap
	angle -= 90.0;
#endif

    return angle;
}

static void angleToJoystick_Clockwise(double angle, longTuple* tuple){
	tuple->x_axis = (XRIGHT*(cos(angle*(PI / 180.0)) + 1)) / 2.0;
	tuple->y_axis = (YBOTTOM*(sin(angle*(PI / 180.0)) + 1)) / 2.0;
}

static void angleToJoystick_CounterClockwise(double angle, longTuple* tuple){
	tuple->x_axis = (XRIGHT  * (cos(angle*(PI/180.0)+(PI/2.0)) + 1) ) / 2.0;
	tuple->y_axis = (YBOTTOM * (sin(angle*(PI/180.0)-(PI/2.0)) + 1) ) / 2.0;
}

/*
Basic Polar to Cartesian conversion

this will return a tuple of 2 values each in the range 0x1-0x8000(32768).
The first is the x direction, which has 1 as leftmost and 32768 as rightmost
second is y, which has 1 as topmost and 32768 as bottommost

MUST LOCK CAMERA for movement to work. it rotates with your movement direction, which messes with it.
aligning camera with 0 on rotation x points us along y axis, facing positive, and enemy moves clockwise around us
*/
void angleToJoystick(double angle, longTuple* tuple){
#if OolicelMap
    angleToJoystick_CounterClockwise(angle, tuple);
#else
	angleToJoystick_Clockwise(angle, tuple);
#endif
}
//get current camera details to lock
void readCamera(HANDLE * processHandle, ullong memorybase){
	camera = malloc(sizeof(CameraSett));
	HANDLE pHandle = (*processHandle);
	//get camera base address
	camera_base += memorybase;

	//get final address
	camera->cam_y_addr = FindPointerAddr(pHandle, camera_base, camera_offsets_length, camera_y_offsets);
	//read y location
	ReadProcessMemory(pHandle, (LPCVOID)(camera->cam_y_addr), &(camera->cam_y), 4, 0);

	//get final address
	camera->cam_x_addr = FindPointerAddr(pHandle, camera_base, camera_offsets_length, camera_x_offsets);
	//read x location
	ReadProcessMemory(pHandle, (LPCVOID)(camera->cam_x_addr), &(camera->cam_x), 4, 0);

	//get rotation addresses
	camera->rot_y_addr = FindPointerAddr(pHandle, camera_base, camera_offsets_length, camera_y_rot_offsets);
	camera->rot_x_addr = FindPointerAddr(pHandle, camera_base, camera_offsets_length, camera_x_rot_offsets);
}

//set the camera to a fixed position and rotation.
void lockCamera(HANDLE * processHandle){
	//TODO do i need to attach to process in order to write?
	HANDLE processHandle_nonPoint = *processHandle;
	//set x location
	/*WriteProcessMemory(processHandle_nonPoint, (LPCVOID)(camera->cam_x_addr), (LPCVOID)(camera->cam_x), 4, NULL);
	//set y location
	WriteProcessMemory(processHandle_nonPoint, (LPCVOID)(camera->cam_y_addr), (LPCVOID)(camera->cam_y), 4, NULL);
	//set x rotation to ???
	float pi = PI;
	WriteProcessMemory(processHandle_nonPoint, (LPCVOID)(camera->rot_x_addr), &pi, 4, NULL);
	//set y rotation to anything, this doesnt matter*/

}


/*#include <stdio.h>//printf

int main(void){
	longTuple a = CoordsToJoystickAngle(26.08102417, 31.13756943, -16.64873314, -17.59091759);
	printf("x: %i y: %i \n", a.first, a.second);
	double x = ((a.first / (double)32768) * (double)100);
	double y = ((a.second / (double)32768) * (double)100);
	printf("x: %f y: %f \n----------------------\n", x, y);
	return 0;
}*/