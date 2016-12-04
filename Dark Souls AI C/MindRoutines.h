//Since the neural networks are threaded, they cannot interface with the primary logic directly.
//Neural network thinking processes are called, and when they return they set flags here marking their desicion
//These flags are then read in the primary logic
//This is done to reduce delay by keeping immediate actions, and allowing complex actions to still be used, hopeful before they become outdated 

#ifndef MindRoutine_H
#define MindRoutine_H

#include "gui.h"
#include <stdbool.h>
#include <Windows.h>
#include "fann.h"
#include "CharacterStruct.h"
#include "SubRoutines.h"
#include "HelperUtil.h"
#include "Memory.h"
#include "Settings.h"

typedef struct{
    struct fann* mind;
    volatile bool exit;
    CRITICAL_SECTION crit;
    CONDITION_VARIABLE cond;
    volatile bool runNetwork;
} MindInput;

extern MindInput* defense_mind_input;

DWORD WINAPI DefenseMindProcess(void* data);

extern volatile char DefenseChoice;


extern MindInput* attack_mind_input;

DWORD WINAPI AttackMindProcess(void* data);

extern volatile unsigned char AttackChoice;

//Helper Methods
int ReadyThreads();

void WaitForThread(MindInput* input);

void WakeThread(MindInput* input);

#endif