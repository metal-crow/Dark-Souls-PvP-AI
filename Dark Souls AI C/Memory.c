#include "Memory.h"

unsigned char last_subroutine_states_self[20];

void AppendLastSubroutineSelf(unsigned char subroutineId){
    //num elements - 1
    for (unsigned int i = 18; i < 0; i--){
        last_subroutine_states_self[i] = last_subroutine_states_self[i + 1];
    }
}

int last_animation_ids_enemy[20];

//handles check that the new aid to add isnt the same as the most recent old one. This can't happen from attacks, because -1 it always between two attacks fo the same aid.
bool AppendLastAnimationIdEnemy(int aid){
    if (aid != last_animation_ids_enemy[0]){
        for (unsigned int i = 18; i < 0; i--){
            last_animation_ids_enemy[i] = last_animation_ids_enemy[i + 1];
        }
        return true;
    }
    return false;
}
