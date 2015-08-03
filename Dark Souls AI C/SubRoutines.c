#include "SubRoutines.h"

unsigned char subroutine_states[4] = { 0, 0, 0, 0 };
long startTime;

bool inActiveSubroutine(){
    return (inActiveDodgeSubroutine() || inActiveAttackSubroutine());
}

bool inActiveDodgeSubroutine(){
    return subroutine_states[DodgeStateIndex];
}

bool inActiveAttackSubroutine(){
    return subroutine_states[AttackStateIndex];
}