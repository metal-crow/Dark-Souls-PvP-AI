#ifndef AnimationMappings_H
#define AnimationMappings_H

#include "gui.h"
#include <stdio.h>

unsigned char isAttackAnimation(unsigned short animationType_id);

unsigned char isDodgeAnimation(unsigned short animationType_id);

unsigned char isVulnerableAnimation(int animation_id);

unsigned char CombineLastAnimation(int animation_id);

float dodgeTimings(int animation_id);

#endif