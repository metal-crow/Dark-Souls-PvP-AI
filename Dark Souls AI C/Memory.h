#ifndef Memory_H
#define Memory_H

#include <stdbool.h>
#include <time.h>

#define last_subroutine_states_self_LENGTH 20
extern unsigned char last_subroutine_states_self[last_subroutine_states_self_LENGTH];

void AppendLastSubroutineSelf(unsigned char subroutineId);

#define last_animation_ids_enemy_LENGTH 20
extern int last_animation_ids_enemy[last_animation_ids_enemy_LENGTH];

bool AppendLastAnimationIdEnemy(int aid);

#define last_animation_types_enemy_LENGTH 100
extern unsigned short last_animation_types_enemy[last_animation_types_enemy_LENGTH];

void AppendAnimationTypeEnemy(unsigned short animationType_id);

#define DistanceMemoryLENGTH 50
extern float DistanceMemory[DistanceMemoryLENGTH];

void AppendDistance(float distance);

#endif