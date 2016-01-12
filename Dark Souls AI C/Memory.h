#ifndef Memory_H
#define Memory_H

#include <stdbool.h>

extern unsigned char last_subroutine_states_self[20];

void AppendLastSubroutineSelf(unsigned char subroutineId);

extern int last_animation_ids_enemy[20];

bool AppendLastAnimationIdEnemy(int aid);

#endif