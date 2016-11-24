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

//enter or continue a dodge subroutine
//this reconciles the MindRoutine and AiDecision choices
//makes deeper decision about what action to take (type of dodge)
void dodge(JOYSTICK_POSITION * iReport, char attackInfo, unsigned char DefenseChoice){

//enter or continue a attack subroutine
//this reconciles the MindRoutine and AiDecision choices
//makes deeper decision about what action to take (type of attack)
void attack(JOYSTICK_POSITION * iReport, char attackInfo, unsigned char AttackNeuralNetChoice);

#endif