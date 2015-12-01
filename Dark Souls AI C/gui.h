#ifndef GUI_H
#define GUI_H

#define _CRT_SECURE_NO_WARNINGS

#include <WinSock2.h>
#include <stdarg.h>
#include <stdio.h>
#pragma comment(lib, "Ws2_32.lib") //load winsock library

#define ENABLEGUI 1
#define ENABLEPRINT 1
#define REDIRECTTOFILE 0

#define LocationMemoryEnemy "0"
#define LocationMemoryPlayer "1"
#define LocationDetection "2"
#define LocationState "3"

int guiStart();

void guiPrint(const char* format, ...);

void guiClose();

#endif