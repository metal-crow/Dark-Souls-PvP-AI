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
extern long startTime;

//find if we are currently in an active subroutine, to prevent simultanious subroutine conflicts
bool inActiveSubroutine();

bool inActiveDodgeSubroutine();

bool inActiveAttackSubroutine();

#endif