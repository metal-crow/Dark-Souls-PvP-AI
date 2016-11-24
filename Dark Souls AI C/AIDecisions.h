#ifndef AIDecisions_H
#define AIDecisions_H

#include "gui.h"
#include "CharacterStruct.h"
#include "SubRoutines.h"
#include "HelperUtil.h"


#define ImminentHit 2
#define EnemyInWindup 1
#define EnemyNeutral 0
#define BehindEnemy -1
#define InBSPosition -2
//this handles actions that override any MindRoutine decisions
char EnemyStateProcessing();

/*typedef enum PriorityDecision_E{
	EnemyNeutral,
	EnterDodgeSubroutine,
	WaitToEnterSubroutine,
	BackstabSubroutine
} PriorityDecision;
PriorityDecision PriorityDecisionMaking(Character * Player, Character * Phantom);*/

#endif