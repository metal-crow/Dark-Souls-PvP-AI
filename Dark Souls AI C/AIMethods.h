#ifndef AIMethods_H
#define AIMethods_H

#include "CharacterStruct.h"
#include "Interface.h"
#include <math.h>
#include <stdbool.h>

//the milliseconds the program sleeps for. Needed to know how long to hold a button so game registeres it
#define sleep_time 1

//return true if i am about to be hit by an attack by the enemy
bool aboutToBeHit(Character * Player, Character * Phantom, unsigned char * subroutine_state);

//initiate the dodge command logic. This can be either toggle escaping, rolling, or parrying.
void dodge(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport, unsigned char * subroutine_state);

//initiate the attack command logic. This can be a standard(physical) attack or a backstab.
void attack(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport, unsigned char * subroutine_state);

#endif