#include "SubRoutines.h"

unsigned char subroutine_states[Num_possible_subroutines] = { 0, 0 };
long startTime;

bool inActiveSubroutine(){
	for (unsigned int i = 0; i < Num_possible_subroutines; i++){
		if (subroutine_states[i]){
			return true;
		}
	}
	return false;
}