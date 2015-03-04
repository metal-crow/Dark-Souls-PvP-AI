#ifndef CharacterStruct_H
#define CharacterStruct_H

#include <stdio.h>
#include <Windows.h>

typedef unsigned long long ullong;

typedef struct {
	//data for x location
	ullong location_x_address;
	float loc_x;
	//data for y location
	ullong location_y_address;
	float loc_y;
	//data for rotation TODO i think this broke
	ullong rotation_address;
	float rotation;
	//data for current animation id
	ullong animation_address;
	unsigned short animation_id;
	//current Right hand weapon they are holding
	ullong r_weapon_address;
	unsigned int r_weapon_id;
	//current left hand weapon they are holding
	ullong l_weapon_address;
	unsigned int l_weapon_id;
	//hurtbox size(range) of weapon. Bows/Magic have high range
	float range;
	//subanimation state. Used to see if hurtbox is active during attack animation
	ullong subanimation_address;
	unsigned short subanimation;

} Character;

//print info about the character
void PrintPhantom(Character * c);

//read memory for the character's variables
void ReadPlayer(Character * c, HANDLE * processHandle);

//basic values and offsets we use
//the base address, which offsets are added to
//this MUST be 64 bits to account for max possible address space
static ullong Enemy_base_add = 0x00F7AC70;
static ullong player_base_add = 0x00F7A7C8;
//offsets and length for x location
static const int Enemy_loc_x_offsets[] = { 0x4, 0x4, 0x2C, 0x260 };
static const int Player_loc_x_offsets[] = { 0x180, 0x1BC, 0x760, 0x8, 0xC70 };
static const int Enemy_loc_x_offsets_length = 4;
static const int Player_loc_x_offsets_length = 5;
//offsets and length for y location
static const int Enemy_loc_y_offsets[] = { 0x4, 0x4, 0x28, 0x54, 0x268 };
static const int Player_loc_y_offsets[] = { 0x180, 0x1BC, 0x760, 0x8, 0xC78 };
static const int Enemy_loc_y_offsets_length = 5;
static const int Player_loc_y_offsets_length = 5;
//offsets and length for rotation (1 is directly facing opponent)
static const int Enemy_rotation_offsets[] = { 0x4, 0x4, 0x34C, 0x654, 0x408 };
static const int Player_rotation_offsets[] = { 0x180, 0x1BC, 0x760, 0x8, 0xD38 };
static const int Enemy_rotation_offsets_length = 5;
static const int Player_rotation_offsets_length = 5;
//offsets and length for animation id
static const int Enemy_animation_offsets[] = { 0x4, 0x4, 0x28, 0x54, 0x1EC };
static const int Player_animation_offsets[] = { 0x180, 0x1BC, 0x760, 0x8, 0xC7C };
static const int Enemy_animation_offsets_length = 5;
static const int Player_animation_offsets_length = 5;
//R weapon id
static const int Enemy_r_weapon_offsets[] = { 0x4, 0x4, 0x34C, 0x654, 0x1F8 };
static const int Player_r_weapon_offsets[] = { 0x4, 0x0, 0x654, 0x1F8 };
static const int Enemy_r_weapon_offsets_length = 5;
static const int Player_r_weapon_offsets_length = 4;
//L weapon id
static const int Enemy_l_weapon_offsets[] = { 0x4, 0x4, 0x34C, 0x654, 0x1B8 };
static const int Player_l_weapon_offsets[] = { 0x4, 0x0, 0x654, 0x1B4 };
static const int Enemy_l_weapon_offsets_length = 5;
static const int Player_l_weapon_offsets_length = 4;
//the current subanimation being executed (1 netural, 0 windup, 256 attack, 257 recovery,)
static const int Enemy_subanimation_offsets[] = { 0x4, 0x4, 0x28, 0x2C, 0x106 };
static const int Player_subanimation_offsets[] = { 0x180, 0x1BC, 0x760, 0x8, 0x10B6 };
static const int Enemy_subanimation_offsets_length = 5;
static const int Player_subanimation_offsets_length = 5;

#endif