#ifndef Interface_H
#define Interface_H

#include <stdio.h>
#include <Windows.h>
#include <math.h>
#include <stdbool.h>

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
#define circle 0x8
#define cross 0x4
#define square 0x1
#define triangle 0x2
#define r1 0x20
#define l1 0x10
#define r2 0x80
#define r3 0x200
#define select 0x400
#define start 0x800

typedef struct{
	long first;
	long second;
} longTuple;

float distance(Character * Player, Character * Phantom);

float angleDeltaFromFront(Character * Player, Character * Phantom);

char BackstabDetection(Character * Player, Character * Phantom, float distance);

double angleFromCoordinates(float player_x, float phantom_x, float player_y, float phantom_y);

float rotationDifferenceFromSelf(Character * Player, Character * Phantom);

longTuple angleToJoystick(double angle);

void readCamera(HANDLE * processHandle, ullong memorybase);

void lockCamera(HANDLE * processHandle);

#endif