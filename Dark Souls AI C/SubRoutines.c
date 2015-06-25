#include "SubRoutines.h"

unsigned char subroutine_states[4] = { 0, 0, 0, 0 };
long startTime;

bool inActiveSubroutine(){
    return (subroutine_states[DodgeStateIndex] || subroutine_states[AttackStateIndex]);
}