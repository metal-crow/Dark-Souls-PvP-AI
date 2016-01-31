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
#define StandardRollId 1
#define BackstepId 2
#define CounterStrafeId 3
#define L1AttackId 5
#define ReverseRollBSId 6
#define ToggleEscapeId 7
#define PerfectBlockId 8
#define ParryId 9
//Attack Ids
#define MoveUpId 1
#define GhostHitId 2
#define BackstabId 3
#define TwoHandId 4
#define SwitchWeaponId 5

//find if we are currently in an active subroutine, to prevent simultanious subroutine conflicts
bool inActiveSubroutine();

bool inActiveDodgeSubroutine();

bool inActiveAttackSubroutine();

void OverrideLowPriorityDefenseSubroutines();

void OverrideLowPriorityAttackSubroutines();

//handles aborting low priority subroutines in case of immediate nesiccary change
//NOTE ensure this isnt called and then the same overridden subroutine isnt retriggered
void OverrideLowPrioritySubroutines();

#endif