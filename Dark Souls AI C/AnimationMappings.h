#ifndef AnimationMappings_H
#define AnimationMappings_H

#include "gui.h"
#include <stdio.h>

enum AnimationTypesE{
    Nothing = 0,
    EatItem = 1,
    CrushUseItem = 15,
    EstusSwig_part1 = 16,
    EstusSwig_part2 = 18,
    EstusSwig_part3 = 19,
    CouldntUse = 21,
    Roll_1H = 32,
    RollBackwards_1H = 33,
    RollRight_1H = 34,
    RollLeft_1H = 35,
    Backstep_1H = 38,
    RollingAttack_1H = 41,
    ShieldPoke = 45,
    R1_1H = 46,
    R1_1H_Combo1 = 48,
    R1_1H_Combo2 = 49,
    Kick_1H = 52,
    Jumping_1H = 53,
    LeftHandAttack = 55,
    Backstep_Attack_1H = 59,
    Backstep_Attack_2H = 60,
    Bow_Draw_2H = 65,
    Bow_Hold = 66,
    Bow_Release_2H = 67,
    Crossbow_Recover_2H = 68,
    Crossbow_Draw_1H = 69,
    Crossbow_Release_1H = 70,
    Crossbow_Recover_1H = 71,
    ShieldRaise_walking = 75,
    ShieldRaise = 76,
    Shield_Held_Up = 77,
    Shield_Lower = 78,
    Shield_Lower_walking = 79,
    Shield_Held_Up_walking_Start = 80,
    Shield_Held_Up_walking = 81,
    Parry = 86,
    R1_1H_into_R2 = 88,
    R2_1H = 89,
    R2_1H_Combo1 = 90,
    Roll_2H = 94,
    RollBackwards_2H = 95,
    RollRight_2H = 96,
    RollLeft_2H = 97,
    Backstep_2H = 100,
    RollingAttack_2H = 103,
    R1_2H = 107,
    R1_2H_Combo1 = 109,
    R1_2H_Combo2 = 110,
    Kick_2H = 112,
    Jumping_2H = 113,
    R1_2H_into_R2 = 114,
    R2_2H = 115,
    R2_2H_Combo1 = 116,
    FireSurge_Windup = 135,
    FireSurge_Cast = 136,
    FireSurge_Recover = 137,
    Miricle_Projectile_Windup = 143,
    Miricle_Projectile_Cast = 144,
    Miricle_AOE_Windup = 145,
    Miricle_AOE_Cast = 146,
    Miricle_Ground_Windup = 155,
    Miricle_Ground_Cast = 156,
    Magic_Cast_Windup = 157,
    Magic_Cast_Cast = 158,
    Magic_Homing_Windup = 159,
    Magic_Homing_Ready = 160,
    FireWhip_Windup = 161,
    FireWhip_Cast = 162,
    FireStorm_Windup = 163,
    FireStorm_Cast = 164,
    Combustion_Windup = 167,
    Combustion_Cast = 168,
    FireBall_Windup = 173,
    FireBall_Cast = 174,
    OneHandWeapon_part1 = 185,
    OneHandWeapon_part2 = 188,
    TwoHandWeapon_part1 = 199,
    TwoHandWeapon_part2 = 203,
    Toggle_R_Weapon_part1 = 209,
    Toggle_R_Weapon_part2 = 213,
    Toggle_L_Weapon_part1 = 217,
    Toggle_L_Weapon_part2 = 221,
    Backstab = 225
} AnimationTypes;

unsigned char isAttackAnimation(unsigned short animationType_id);

unsigned char isDodgeAnimation(unsigned short animationType_id);

unsigned char isVulnerableAnimation(int animation_id);

unsigned char CombineLastAnimation(int animation_id);

float dodgeTimings(int animation_id);

#endif