#include "SubRoutines.h"

unsigned char subroutine_states[4] = { 0, 0, 0, 0 };
long startTime;

bool inActiveSubroutine(){
    return (inActiveDodgeSubroutine() || inActiveAttackSubroutine());
}

//special case for preventing unneccicary redodge
bool inActiveDodgeSubroutine(){
    return subroutine_states[DodgeStateIndex]>1;
}

bool inActiveAttackSubroutine(){
    return subroutine_states[AttackStateIndex];
}