#ifndef CharacterStruct_H
#define CharacterStruct_H

#include "gui.h"
#include <stdio.h>
#include <Windows.h>
#include "AnimationMappings.h"

typedef unsigned long long ullong;

typedef struct {
	//data for x location
	ullong location_x_address;
	float loc_x;
	//data for y location
	ullong location_y_address;
	float loc_y;
	//data for rotation
	ullong rotation_address;
	float rotation;
	//data for current animation id
	ullong animation_address;
	unsigned int animation_id;
	//hp
	ullong hp_address;
	unsigned int hp;
    //stamina
    ullong stamina_address;
    int stamina;
	//current Right hand weapon they are holding
	ullong r_weapon_address;
	unsigned int r_weapon_id;
	//current left hand weapon they are holding
	ullong l_weapon_address;
	unsigned int l_weapon_id;
	//hurtbox size(range) of weapon. Bows/Magic have high range
	float weaponRange;
    //encompases the various states of an animation
    unsigned int subanimation;
    //the current attack animation id
    ullong attackAnimationId_address;
	//animation timer.
    ullong animationTimer_address;
	//hurtbox state 
    ullong hurtboxActive_address;
    //windup state
    ullong windup_address;
    //recovery state
    ullong recoveryState_address;
    //velocity. used for backstab detection
    ullong velocity_address;
    float velocity;
    //if locked on. used for verification for counter strafe
    ullong locked_on_address;
    unsigned char locked_on;
} Character;

//initalize the phantom and player
Character Enemy;
Character Player;

//read memory for the character's variables
void ReadPlayer(Character * c, HANDLE * processHandle, int Character);

void ReadPlayerDEBUGGING(Character * c, HANDLE * processHandle, ...);

//TODO prune as many of these as possible. what needs to be kept for only one char?

//basic values and offsets we use
//the base address, which offsets are added to
//this MUST be 64 bits to account for max possible address space
static ullong Enemy_base_add = 0x00F7DC70;
static ullong player_base_add = 0x00F7D644;
//offsets and length for x location
static const int Enemy_loc_x_offsets[] = { 0x4, 0x4, 0x2C, 0x260 };
static const int Player_loc_x_offsets[] = { 0x3C, 0x330, 0x4, 0x20C, 0x3C0 };
static const int Enemy_loc_x_offsets_length = 4;
static const int Player_loc_x_offsets_length = 5;
//offsets and length for y location
static const int Enemy_loc_y_offsets[] = { 0x4, 0x4, 0x28, 0x54, 0x268 };
static const int Player_loc_y_offsets[] = { 0x3C, 0x330, 0x4, 0x20C, 0x3C8 };
static const int Enemy_loc_y_offsets_length = 5;
static const int Player_loc_y_offsets_length = 5;
//offsets and length for rotation.
static const int Enemy_rotation_offsets[] = { 0x4, 0x4, 0x28, 0x54, 0x34 };
static const int Player_rotation_offsets[] = { 0x288, 0xC0, 0x4, 0x18, 0x4 };
static const int Enemy_rotation_offsets_length = 5;
static const int Player_rotation_offsets_length = 5;
//offsets and length for animation id
static const int Enemy_animation_offsets[] = { 0x4, 0x4, 0x28, 0x54, 0x1EC };
static const int Player_animation_offsets[] = { 0x288, 0xC, 0xC, 0x10, 0x41C };
static const int Enemy_animation_offsets_length = 5;
static const int Player_animation_offsets_length = 5;
//hp
static const int Enemy_hp_offsets[] = { 0x4, 0x4, 0x2D4 };
static const int Player_hp_offsets[] = { 0x288, 0xC, 0x330, 0x4, 0x2D4 };
static const int Enemy_hp_offsets_length = 3;
static const int Player_hp_offsets_length = 5;
//stamina
static const int Player_stamina_offsets[] = { 0x288, 0xC, 0x330, 0x4, 0x2E4 };
static const int Player_stamina_offsets_length = 5;
//R weapon id
static const int Enemy_r_weapon_offsets[] = { 0x4, 0x4, 0x34C, 0x654, 0x1F8 };
static const int Player_r_weapon_offsets[] = { 0x3C, 0x30, 0xC, 0x654, 0x1F8 };
static const int Enemy_r_weapon_offsets_length = 5;
static const int Player_r_weapon_offsets_length = 5;
//L weapon id
static const int Enemy_l_weapon_offsets[] = { 0x4, 0x4, 0x34C, 0x654, 0x1B8 };
static const int Player_l_weapon_offsets[] = { 0x3C, 0x30, 0xC, 0x654, 0x1B4 };
static const int Enemy_l_weapon_offsets_length = 5;
static const int Player_l_weapon_offsets_length = 5;
//the current subanimation being executed
#define AttackSubanimationWindup 00
#define AttackSubanimationWindupClosing 02
#define AttackSubanimationActiveDuringHurtbox 11
#define AttackSubanimationActiveHurtboxOver 12
#define AttackSubanimationRecover 20
#define SubanimationNeutral 30
//windup state
static const int Enemy_windup_offsets[] = { 0x4, 0x4, 0x28, 0x2C, 0x107 };
static const int Enemy_windup_offsets_length = 5;
//if enemy's weapon's hurtbox is active
static const int Enemy_hurtboxActive_offsets[] = { 0x4, 0x0, 0xC, 0x3C, 0xF };
static const int Enemy_hurtboxActive_offsets_length = 5;

//if windup is about to close
//TODO this one isnt the best either...
//byte doesnt work. try 4 byte or float
//static const int Enemy_windupClose_offsets[] = { 0x4, 0x4, 0x24, 0x5C, 0x16 };//{ 0x4, 0x4, 0x658, 0x5C, 0xEB };
//static const int Player_windupClose_offsets[] = { 0x28, 0x0, 0x34C, 0x24, 0x1BB };//TODO
//static const int Enemy_windupClose_offsets_length = 5;
//static const int Player_windupClose_offsets_length = 5;

//time animation has been active
static const int Enemy_animationTimer_offsets[] = { 0x4, 0x4, 0x28, 0x18, 0x4DC };
static const int Enemy_animationTimer_offsets_length = 5;
//current enemy attack animation id
static const int Enemy_attackAnimationID_offsets[] = { 0x4, 0x4, 0x28, 0x18, 0x444 };
static const int Enemy_attackAnimationID_offsets_length = 5;
//if in recover state
static const int Player_recoverState_offsets[] = { 0x3C, 0x60, 0x168, 0x2C, 0x415 };
static const int Player_recoverState_offsets_length = 5;
//speed the opponent is approaching at. Player doesnt need to know their own. Idealy would like just if sprinting or not, actual velocity isnt important
//-0.04 slow walk
//-0.13 walk
//-0.16 - 18 sprint
static const int Enemy_velocity_offsets[] = { 0x4, 0x4, 0x658, 0x5C, 0x3BC };
static const int Enemy_velocity_offsets_length = 5;
//if player is locked on. used for verification only
static const int Player_Lock_on_offsets[] = { 0x3C, 0x170, 0x2C, 0x390, 0x128 };
static const int Player_Lock_on_offsets_length = 5;
#endif