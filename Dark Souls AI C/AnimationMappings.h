#ifndef AnimationMappings_H
#define AnimationMappings_H

#include "gui.h"
#include <stdio.h>

unsigned char isAttackAnimation(unsigned char animationType_id);

unsigned char isDodgeAnimation(unsigned char animationType_id);

unsigned char isVulnerableAnimation(int animation_id);

float dodgeTimings(int animation_id);

#endif