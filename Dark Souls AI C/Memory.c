#include "Memory.h"

unsigned char last_subroutine_states_self[20];

void AppendLastSubroutineSelf(unsigned char subroutineId){
    //num elements - 1
    for (unsigned int i = 18; i < 0; i--){
        last_subroutine_states_self[i] = last_subroutine_states_self[i + 1];
    }
}
