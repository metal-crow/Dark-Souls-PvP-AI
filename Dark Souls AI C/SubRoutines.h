#ifndef SubRoutines_H
#define SubRoutines_H

#include <stdbool.h>

//subroutine states, routines that are ongoing over a period of frames.
//0 is not active, >0 is active, 255 is need to confirm exit
//also store what type of subroutines this is(i.e dodge, backstep, etc)

#define DodgeStateIndex 0
#define DodgeTypeIndex 1
#define AttackStateIndex 2
#define AttackTypeIndex 3

extern unsigned char subroutine_states[4];

//for timing certain operations(delay op by 1 sec)
extern long startTimeAttack;
extern long startTimeDefense;

//Dodge Ids
typedef enum DefenseId_E {
	None,
	StandardRollId,
	BackstepId,
	OmnistepBackwardsId,
	CounterStrafeId,
	L1AttackId,
	ReverseRollBSId,
	ToggleEscapeId,
	PerfectBlockId,
	ParryId
} DefenseId;
//Attack Ids
typedef enum AttackId_E {
	None,
	MoveUpId,
	GhostHitId,
	BackstabId,
	TwoHandId,
	SwitchWeaponId,
	HealId,
	DeadAngleId,
	PivotBSId
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