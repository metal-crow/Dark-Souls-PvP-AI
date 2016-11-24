#ifndef AIMethods_H
#define AIMethods_H

//this composes the INSTINCT of the ai mind. Basic interations/actions/reactions that are high accuracy, very fast, but not very complex.

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
void dodge(JOYSTICK_POSITION * iReport, char attackInfo, unsigned char DefenseChoice){

//enter or continue a attack subroutine
//this reconciles the MindRoutine and AiDecision choices
void attack(JOYSTICK_POSITION * iReport, char attackInfo, unsigned char AttackNeuralNetChoice);

#endif