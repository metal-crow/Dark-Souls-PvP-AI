#ifndef Interface_H
#define Interface_H

#include <stdio.h>
#include <Windows.h>
#include <math.h>

#include "vjoyinterface.h"
#include "public.h"
#include "MemoryEdits.h"
#include "CharacterStruct.h"

#define PI 3.14159265
#define XRIGHT 32768
#define XLEFT 1
#define YTOP 1
#define YBOTTOM 32768
#define MIDDLE 16384

int loadvJoy(UINT iInterface);

//ps3 controller mapping
#define r1 0x000000020
#define circle 0x00000008

typedef struct{
	long first;
	long second;
} longTuple;

//distance between two chars as the crow flies
float distance(Character * Player, Character * Phantom);

//the absolute value of the angle the opponent is off from straight ahead (returns radians, only used as neural net input)
float angleDeltaFromFront(Character * Player, Character * Phantom);

//given player and enemy coordinates, get the angle between the two
double angleFromCoordinates(float player_x, float phantom_x, float player_y, float phantom_y);

float rotationDifferenceFromSelf(Character * Player, Character * Phantom);

/*this will return a tuple of 2 values each in the range 0x1-0x8000(32768).
The first is the x direction, which has 1 as leftmost and 32768 as rightmost
second is y, which has 1 as topmost and 32768 as bottommost

MUST LOCK CAMERA for movement to work. it rotates with your movement direction, which messes with it.
aligning camera with 0 on rotation x points us along y axis, facing positive, and enemy moves clockwise around us*/
longTuple angleToJoystick(double angle);

//get current camera details to lock
void readCamera(HANDLE * processHandle, ullong memorybase);

//set the camera to a fixed position and rotation.
void lockCamera(HANDLE * processHandle);

#endif