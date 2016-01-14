#ifndef Source_H
#define Source_H

#include "gui.h"
#include "MemoryEdits.h"
#include "CharacterStruct.h"
#include "HelperUtil.h"
#include "AIMethods.h"
#include "SubRoutines.h"
#include "MindRoutines.h"

#include "fann.h"

extern HANDLE processHandle;

int SetupandLoad();

void MainLogicLoop();

void Exit();

#endif