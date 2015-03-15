#include "Interface.h"
#include <stdio.h>//printf

#pragma warning( disable: 4244 )//ignore dataloss conversion from double to long

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

longTuple CoordsToJoystickAngle(float player_x, float phantom_x, float player_y, float phantom_y){
	float delta_x = phantom_x - player_x;
	float delta_y = player_y - phantom_y;

	long joystick_x;
	long joystick_y;

	//need to calibrate player direction by starting them centered along an axis
	//aligning with pi points us along x axis, and enemy moves clockwise around us

	double angle = atan2(delta_y, delta_x) * 180 / PI;

	//top right quadrant
	if ((int)angle <= 90 && (int)angle >= 0){
		printf("top right%f\n", angle);
		if (angle <= 45){
			joystick_x = XRIGHT;
			joystick_y = MIDDLE - ((angle * MIDDLE) / 45);
		} else{
			joystick_y = YTOP;
			joystick_x = XRIGHT - ((abs(angle - 45) * MIDDLE) / 45);
		}
	}
	//bottom right quadrant
	else if ((int)angle <= 180 && (int)angle >= 90){
		printf("bottom right%f\n", angle);
		double anglediff = abs(angle - 135);
		if (anglediff <= 45){
			joystick_x = XRIGHT - ((anglediff * MIDDLE) / 45);
			joystick_y = YBOTTOM;
		} else{
			joystick_y = MIDDLE + ((anglediff * MIDDLE) / 45);
			joystick_x = XRIGHT;
		}
	}
	//bottom left quadrant
	else if ((int)angle <= 270 && (int)angle >= 180){
		printf("bottom left%f\n", angle);
		double anglediff = abs(angle - 225);
		if (anglediff <= 45){
			joystick_x = XLEFT + ((anglediff * MIDDLE) / 45);
			joystick_y = YTOP;
		} else{
			joystick_y = YTOP + ((anglediff * MIDDLE) / 45);
			joystick_x = XLEFT;
		}
	//top left quadrant
	} else{
		printf("top left%f\n", angle);
		double anglediff = abs(angle - 315);
		if (anglediff <= 45){
			joystick_x = XLEFT;
			joystick_y = YBOTTOM - ((anglediff * MIDDLE) / 45);
		} else{
			joystick_y = YBOTTOM;
			joystick_x = XLEFT + ((anglediff * MIDDLE) / 45);
		}
	}

	longTuple tuple;
	tuple.first = joystick_x;
	tuple.second = joystick_y;

	return tuple;
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