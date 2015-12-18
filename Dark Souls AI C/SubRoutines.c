#include "SubRoutines.h"

unsigned char subroutine_states[4] = { 0, 0, 0, 0 };
//allow attack and defense subroutines to operate independently
long startTimeAttack;
long startTimeDefense;

bool inActiveSubroutine(){
    return (inActiveDodgeSubroutine() || inActiveAttackSubroutine());
}

bool inActiveDodgeSubroutine(){
    return subroutine_states[DodgeStateIndex];
}

bool inActiveAttackSubroutine(){
    return subroutine_states[AttackStateIndex];
}

//if we're currently strafing and get attacked, break strafe subroutine
void OverrideStrafeSubroutine(){
    if (subroutine_states[DodgeTypeIndex] == 3){
        subroutine_states[DodgeTypeIndex] = 0;
        subroutine_states[DodgeStateIndex] = 0;
    }
}