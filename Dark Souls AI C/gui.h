#ifndef GUI_H
#define GUI_H

#define _CRT_SECURE_NO_WARNINGS

#include <WinSock2.h>
#include <stdarg.h>
#include <stdio.h>
#include "CharacterStruct.h"
#pragma comment(lib, "Ws2_32.lib") //load winsock library

#define LocationMemoryEnemy EnemyId
#define LocationMemoryPlayer PlayerId
#define LocationDetection "2"
#define LocationState "3"
#define LocationJoystick "4"
#define LocationHandler "5"

int guiStart();

void guiPrint(const char* format, ...);

void guiClose();

#endif