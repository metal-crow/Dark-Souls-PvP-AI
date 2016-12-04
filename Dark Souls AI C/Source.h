#ifndef Source_H
#define Source_H

#include "gui.h"
#include "MemoryEdits.h"
#include "CharacterStruct.h"
#include "HelperUtil.h"
#include "AIMethods.h"
#include "AIDecisions.h"
#include "SubRoutines.h"
#include "MindRoutines.h"
#include "vjoyhelper.h"

#include "fann.h"

extern HANDLE processHandle;
extern ullong memorybase;

#define iInterface 1// Default target vJoy device
extern JOYSTICK_POSITION iReport;

int SetupandLoad();

void MainLogicLoop();

void Exit();

#endif