#ifndef SubRoutines_H
#define SubRoutines_H

#include <stdbool.h>

#define Num_possible_subroutines 2

//subroutine states, routines that are ongoing over a period of frames.
//each numbered state corrisponds to a frame
//0 is not active, >0 is active, 255 is need to confirm exit
//indexes:0 is dodge, 1 is attack
extern unsigned char subroutine_states[Num_possible_subroutines];

//find if we are currently in an active subroutine, to prevent simultanious subroutine conflicts
bool inActiveSubroutine();

#endif