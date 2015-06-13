#ifndef AIMethods_H
#define AIMethods_H

//this composes the INSTINCT of the ai mind. Basic interations/actions/reactions that are high accuracy, very fast, but not very complex.

#include "CharacterStruct.h"
#include "Interface.h"
#include <math.h>
#include <stdbool.h>
#include "SubRoutines.h"
#include <time.h>

#define AttackSubanimationWindup 0
#define AttackSubanimationActive 65792
#define AttackSubanimationRecover 65793

//return 2 if i am about to be hit by an attack by the enemy, 1 if an attack if coming soon and i should wait to dodge it, 0 otherwise
unsigned char aboutToBeHit(Character * Player, Character * Phantom);

//initiate the dodge command logic. This can be either toggle escaping, rolling, or parrying.
void dodge(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport);

//initiate the attack command logic. This can be a standard(physical) attack or a backstab.
void attack(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport);

#endif