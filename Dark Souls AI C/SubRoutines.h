#ifndef SubRoutines_H
#define SubRoutines_H

#include <stdbool.h>

//subroutine states, routines that are ongoing over a period of frames.
//store what subroutine is active (defence/attack and the subroutine id), and what state it is in 

#define DodgeStateIndex 0
#define DodgeTypeIndex 1
#define AttackStateIndex 2
#define AttackTypeIndex 3

extern unsigned char subroutine_states[4];

//for timing subroutine operations
extern long startTimeAttack;
extern long startTimeDefense;

//Dodge Ids
typedef enum DefenseId_E {
	DefNoneId,//should only be used for initalizing. Should never reach AiMethod code
	StandardRollId,
	BackstepId,
	OmnistepBackwardsId,
	CounterStrafeLeftId,
	CounterStrafeRightId,
	L1AttackId,
	ReverseRollBSId,
	ToggleEscapeId,
	PerfectBlockId,
	ParryId,
} DefenseId;
//Attack Ids
typedef enum AttackId_E {
	AtkNoneId,//should only be used for initalizing. Should never reach AiMethod code
	MoveUpId,
	GhostHitId,
	DeadAngleId,
	BackstabId,
	TwoHandId,
	SwitchWeaponId,
	HealId,
	PivotBSId,
} AttackId;

#define SubroutineActive 1
#define SubroutineExiting 2
#define NoSubroutineActive 0

bool inActiveSubroutine();

bool inActiveDodgeSubroutine();

bool inActiveAttackSubroutine();

void OverrideLowPriorityDefenseSubroutines();

void OverrideLowPriorityAttackSubroutines();

void OverrideLowPrioritySubroutines();

void SafelyExitSubroutines();

#endif