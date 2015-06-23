#include "Interface.h"
#include <stdio.h>//printf

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

//get straight line distance between me and enemy
double distance(Character * Player, Character * Phantom){
    double delta_x = fabsf(fabsf(Player->loc_x) - fabsf(Phantom->loc_x));
    double delta_y = fabsf(fabsf(Player->loc_y) - fabsf(Phantom->loc_y));
    return hypot(delta_x, delta_y);
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
	BOOL AxisRZ = GetVJDAxisExist(iInterface, HID_USAGE_RZ);
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
	printf("Axis Rz\t\t%s\n", AxisRZ ? "Yes" : "No");

	// Acquire the target
	if ((status == VJD_STAT_OWN) || ((status == VJD_STAT_FREE) && (!AcquireVJD(iInterface)))){
		printf("Failed to acquire vJoy device number %d.\n", iInterface);
		return -1;
	} else{
		printf("Acquired: vJoy device number %d.\n", iInterface);
	}
	return 0;
}

double angleFromCoordinates(float player_x, float phantom_x, float player_y, float phantom_y){
	double delta_x = fabsf(player_x) - fabsf(phantom_x);
	double delta_y = fabsf(phantom_y) - fabsf(player_y);

	//convert this to 360 degrees
	double angle = (atan2(delta_x, delta_y) + PI) * (180.0 / PI);

	return angle;
}

longTuple angleToJoystick(double angle){
	longTuple tuple;

	//top right quadrant
	if (angle <= 90 && angle >= 0){
		if (angle <= 45){
			tuple.first = MIDDLE + ((angle * MIDDLE) / 45);
			tuple.second = YTOP;
		} else{
			tuple.second = YTOP + (((angle - 45.0) * MIDDLE) / 45);
			tuple.first = XRIGHT;
		}
	}
	//bottom right quadrant
	else if (angle <= 180 && angle >= 90){
		double anglediff = fabs(angle - 90.0);
		if (anglediff <= 45){
			tuple.first = XRIGHT;
			tuple.second = MIDDLE + ((anglediff * MIDDLE) / 45);
		} else{
			tuple.second = YBOTTOM;
			tuple.first = XRIGHT - (((anglediff - 45.0) * MIDDLE) / 45);
		}
	}
	//bottom left quadrant
	else if (angle <= 270 && angle >= 180){
		double anglediff = fabs(angle - 180.0);
		if (anglediff <= 45){
			tuple.first = MIDDLE - ((anglediff * MIDDLE) / 45);
			tuple.second = YBOTTOM;
		} else{
			tuple.second = YBOTTOM - (((anglediff - 45.0) * MIDDLE) / 45);
			tuple.first = XLEFT;
		}
	//top left quadrant
	} else{
		double anglediff = fabs(angle - 270.0);
		if (anglediff <= 45){
			tuple.first = XLEFT;
			tuple.second = MIDDLE - ((anglediff * MIDDLE) / 45);
		} else{
			tuple.second = YTOP;
			tuple.first = XLEFT + (((anglediff - 45.0) * MIDDLE) / 45);
		}
	}

	return tuple;
}

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