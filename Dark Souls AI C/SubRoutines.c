#include "SubRoutines.h"

unsigned char subroutine_states[4] = { 0, 0, 0, 0 };
//allow attack and defense subroutines to operate independently
long startTimeAttack;
long startTimeDefense;

//find if we are currently in an active subroutine, to prevent simultanious subroutine conflicts
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
    if (subroutine_states[AttackTypeIndex] == MoveUpId || subroutine_states[AttackTypeIndex] == SwitchWeaponId){
        subroutine_states[AttackTypeIndex] = NoSubroutineActive;
        subroutine_states[AttackStateIndex] = NoSubroutineActive;
    }
}

void OverrideLowPriorityDefenseSubroutines(){
	if (subroutine_states[DodgeTypeIndex] == CounterStrafeLeftId || subroutine_states[DodgeTypeIndex] == CounterStrafeRightId){
        subroutine_states[DodgeTypeIndex] = NoSubroutineActive;
        subroutine_states[DodgeStateIndex] = NoSubroutineActive;
    }
}

//handles aborting low priority subroutines in case of immediate nesiccary change
//NOTE ensure this isnt called and then the same overridden subroutine isnt retriggered
void OverrideLowPrioritySubroutines(){
    OverrideLowPriorityDefenseSubroutines();
    OverrideLowPriorityAttackSubroutines();
}

//saftly exit all subroutines in the exit state
//done so that a dodge subroutine exit doesnt allow an immediate enty into an attack subroutine until next tick, and dodge rechecks
void SafelyExitSubroutines(){
    if (subroutine_states[DodgeStateIndex] == SubroutineExiting){
        subroutine_states[DodgeStateIndex] = NoSubroutineActive;
        subroutine_states[DodgeTypeIndex] = NoSubroutineActive;
    }
    if (subroutine_states[AttackStateIndex] == SubroutineExiting){
        subroutine_states[AttackStateIndex] = NoSubroutineActive;
        subroutine_states[AttackTypeIndex] = NoSubroutineActive;
    }
}
