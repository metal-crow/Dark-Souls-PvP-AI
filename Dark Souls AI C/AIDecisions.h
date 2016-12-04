#ifndef AIDecisions_H
#define AIDecisions_H

#include "gui.h"
#include "CharacterStruct.h"
#include "SubRoutines.h"
#include "HelperUtil.h"
#include "Settings.h"
#include "AIMethods.h"

typedef enum PriorityDecision_E{
	EnemyNeutral,//doesn't override anything, and doesnt define an attack or defense id
	DelayActions,//doesnt define an attack or defense id
	EnterDodgeSubroutine,//only defines defense ids
	EnterAttackSubroutine,//only defines attack ids
} PriorityDecision;

typedef union SubroutineId_U{
	AttackId attackid;
	DefenseId defenseid;
} SubroutineId;

typedef struct InstinctDecision_S{
	PriorityDecision priority_decision;
	SubroutineId subroutine_id;
} InstinctDecision;

//this handles actions that override any MindRoutine decisions
//makes decisions about what general types of actions the ai should take using standard discrete logic (i.e should dodge, but not what type of dodge)
//composes the INSTINCT of the ai mind. Basic interations/actions/reactions that are high accuracy, very fast, but not very complex.
void InstinctDecisionMaking(InstinctDecision* instinct_decision);

#endif