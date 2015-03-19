#include "SubRoutines.h"

bool inActiveSubroutine(){
	for (unsigned int i = 0; i < Num_possible_subroutines; i++){
		if (subroutine_states[i]){
			return true;
		}
	}
	return false;
}