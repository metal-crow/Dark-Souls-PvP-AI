#include "MindRoutines.h"

DWORD WINAPI DefenseMindProcess(void* data){
    while (!defense_mind_input->exit){
        fann_type* out=fann_run(defense_mind_input->mind, defense_mind_input->input);
        DefenseChoice = (unsigned char)(*out);
    }
    return 0;
}

DWORD WINAPI AttackMindProcess(void* data){
    while (!attack_mind_input->exit){
        fann_type* out = fann_run(attack_mind_input->mind, attack_mind_input->input);
        AttackChoice = (unsigned char)(*out);
    }
    return 0;
}