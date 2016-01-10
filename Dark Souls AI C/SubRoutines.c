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

void OverrideLowPriorityAttackSubroutines(){
    if (subroutine_states[AttackTypeIndex] == MoveUpId){
        subroutine_states[AttackTypeIndex] = 0;
        subroutine_states[AttackStateIndex] = 0;
    }
}

void OverrideLowPriorityDefenseSubroutines(){
    if (subroutine_states[DodgeTypeIndex] == CounterStrafeId){
        subroutine_states[DodgeTypeIndex] = 0;
        subroutine_states[DodgeStateIndex] = 0;
    }
}

void OverrideLowPrioritySubroutines(){
    OverrideLowPriorityDefenseSubroutines();
    OverrideLowPriorityAttackSubroutines();
}
