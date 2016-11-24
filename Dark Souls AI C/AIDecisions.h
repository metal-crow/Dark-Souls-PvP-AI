#ifndef AIDecisions_H
#define AIDecisions_H

#include "gui.h"
#include "CharacterStruct.h"
#include "SubRoutines.h"
#include "HelperUtil.h"

typedef enum PriorityDecision_E{
	EnemyNeutral,//doesn't override anything

	DelayActions,
	EnterDodgeSubroutine,
	EnterAttackSubroutine,
} PriorityDecision;
//this handles actions that override any MindRoutine decisions
//makes decisions about what general types of actions the ai should take using standard discrete logic (i.e should dodge, but not what type of dodge)
//composes the INSTINCT of the ai mind. Basic interations/actions/reactions that are high accuracy, very fast, but not very complex.
PriorityDecision PriorityDecisionMaking();

#endif