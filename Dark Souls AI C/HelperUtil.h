#ifndef HelperUtil_H
#define HelperUtil_H

#include "gui.h"
#include <stdio.h>
#include <Windows.h>
#include <math.h>
#include <stdbool.h>

#include "vjoyinterface.h"
#include "public.h"
#include "MemoryEdits.h"
#include "CharacterStruct.h"
#include "WeaponData.h"

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
#define l2 0x40 //untested
#define r2 0x80
#define l3 0x100 //untested
#define r3 0x200
#define select 0x400
#define start 0x800
#define dup 0x0
#define dright 0x1
#define ddown 0x2
#define dleft 0x3
#define dcenter 0x4

typedef struct{
	long x_axis;
	long y_axis;
} longTuple;

float distance(Character * Player, Character * Phantom);

float angleDeltaFromFront(Character * Player, Character * Phantom);

bool AnglesWithinRange(float ang1, float ang2, float range);

int StaminaEstimationEnemy();

unsigned char BackstabDetection(Character* Perspective, Character* Target, float distance);

double angleFromCoordinates(float player_x, float phantom_x, float player_y, float phantom_y);

float rotationDifferenceFromSelf(Character * Player, Character * Phantom);

void angleToJoystick(double angle, longTuple * tuple);

void readCamera(HANDLE * processHandle, ullong memorybase);

void lockCamera(HANDLE * processHandle);

#endif