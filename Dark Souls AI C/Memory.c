#include "Memory.h"

unsigned char last_subroutine_states_self[20];

void AppendLastSubroutineSelf(unsigned char subroutineId){
    for (unsigned int i = 19; i > 1; i--){
        last_subroutine_states_self[i] = last_subroutine_states_self[i - 1];
    }
    last_subroutine_states_self[0] = subroutineId;
}

int last_animation_ids_enemy[20];

//handles check that the new aid to add isnt the same as the most recent old one. This can't happen from attacks, because -1 it always between two attacks fo the same aid.
bool AppendLastAnimationIdEnemy(int aid){
    if (aid != last_animation_ids_enemy[0]){
        for (unsigned int i = 19; i > 1; i--){
            last_animation_ids_enemy[i] = last_animation_ids_enemy[i - 1];
        }
        last_animation_ids_enemy[0] = aid;
        return true;
    }
    return false;
}
