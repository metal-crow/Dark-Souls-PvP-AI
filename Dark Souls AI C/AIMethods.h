#ifndef AIMethods_H
#define AIMethods_H

#include "gui.h"
#include "CharacterStruct.h"
#include "HelperUtil.h"
#include <math.h>
#include "SubRoutines.h"
#include <time.h>
#include "AnimationMappings.h"
#include "Memory.h"
#include "AIDecisions.h"

#define TimeForR3ToTrigger 50
#define TimeForCameraToRotateAfterLockon 180//how much time we give to allow the camera to rotate.
#define TimeDeltaForGameRegisterAction 170
#define TotalTimeInSectoReverseRoll ((TimeForR3ToTrigger + TimeForCameraToRotateAfterLockon + TimeDeltaForGameRegisterAction + 50) / (float)CLOCKS_PER_SEC)//convert above CLOCKS_PER_SEC ticks to seconds

//enter or continue a dodge subroutine
//this reconciles the MindRoutine and AiDecision choices
//makes deeper decision about what action to take (type of dodge)
void dodge(JOYSTICK_POSITION * iReport, struct InstinctDecision_S* instinct_decision, unsigned char DefenseNeuralNetChoice);

//enter or continue a attack subroutine
//this reconciles the MindRoutine and AiDecision choices
//makes deeper decision about what action to take (type of attack)
void attack(JOYSTICK_POSITION * iReport, struct InstinctDecision_S* instinct_decision, unsigned char AttackNeuralNetChoice);

#endif
