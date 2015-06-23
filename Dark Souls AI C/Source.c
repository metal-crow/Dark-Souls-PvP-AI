#include "MemoryEdits.h"
#include "CharacterStruct.h"
#include "Interface.h"
#include "AIMethods.h"
#include "SubRoutines.h"
#include "MindRoutines.h"

#include "fann.h"

#pragma comment( lib, "VJOYINTERFACE" )//load vjoy library
#define  _CRT_SECURE_NO_WARNINGS

//initalize the phantom and player
Character Enemy;
Character Player;

//intialize extern variables for nerual net
MindInput* defense_mind_input;
volatile unsigned char DefenseChoice;
MindInput* attack_mind_input;
volatile unsigned char AttackChoice;

int main(void){
	//memset to ensure we dont have unusual char attributes at starting
	memset(&Enemy, 0, sizeof(Character));
	Enemy.weaponRange = 2.5;//temp hardcoding
	memset(&Player, 0, sizeof(Character));
	Player.weaponRange = 2.5;

	//get access to dark souls memory
	char * processName = "DARKSOULS.exe";
	//get the process id from the name
	int processId = GetProcessIdFromName(processName);
	//open the handle
	HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, processId);
	//get the base address of the process and append all other addresses onto it
	ullong memorybase = GetModuleBase(processId, processName);
	Enemy_base_add += memorybase;
	player_base_add += memorybase;

	//add the pointer offsets to the address
	Enemy.location_x_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_loc_x_offsets_length, Enemy_loc_x_offsets);
	Enemy.location_y_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_loc_y_offsets_length, Enemy_loc_y_offsets);
	Enemy.rotation_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_rotation_offsets_length, Enemy_rotation_offsets);
	Enemy.animation_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_animation_offsets_length, Enemy_animation_offsets);
	Enemy.hp_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_hp_offsets_length, Enemy_hp_offsets);
	Enemy.r_weapon_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_r_weapon_offsets_length, Enemy_r_weapon_offsets);
	Enemy.l_weapon_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_l_weapon_offsets_length, Enemy_l_weapon_offsets);
	Enemy.subanimation_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_subanimation_offsets_length, Enemy_subanimation_offsets);
	Enemy.weightanimation_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_weightanimation_offsets_length, Enemy_weightanimation_offsets);

	Player.location_x_address = FindPointerAddr(processHandle, player_base_add, Player_loc_x_offsets_length, Player_loc_x_offsets);
	Player.location_y_address = FindPointerAddr(processHandle, player_base_add, Player_loc_y_offsets_length, Player_loc_y_offsets);
	Player.rotation_address = FindPointerAddr(processHandle, player_base_add, Player_rotation_offsets_length, Player_rotation_offsets);
	Player.animation_address = FindPointerAddr(processHandle, player_base_add, Player_animation_offsets_length, Player_animation_offsets);
	Player.hp_address = FindPointerAddr(processHandle, player_base_add, Player_hp_offsets_length, Player_hp_offsets);
	Player.r_weapon_address = FindPointerAddr(processHandle, player_base_add, Player_r_weapon_offsets_length, Player_r_weapon_offsets);
	Player.l_weapon_address = FindPointerAddr(processHandle, player_base_add, Player_l_weapon_offsets_length, Player_l_weapon_offsets);
	Player.subanimation_address = FindPointerAddr(processHandle, player_base_add, Player_subanimation_offsets_length, Player_subanimation_offsets);
	Player.weightanimation_address = FindPointerAddr(processHandle, player_base_add, Player_weightanimation_offsets_length, Player_weightanimation_offsets);

	//want to use controller input, instead of keyboard, as analog stick is more precise movement
	UINT iInterface = 1;								// Default target vJoy device
	int loadresult = loadvJoy(iInterface);
	if (loadresult != 0){
		return loadresult;
	}
	JOYSTICK_POSITION iReport;
	iReport.bDevice = (BYTE)iInterface;

    //load neural network and threads
    defense_mind_input = malloc(sizeof(MindInput));
    struct fann* defense_mind = fann_create_from_file("Defense_dark_souls_ai.net");
    if (defense_mind == NULL){
        printf("Defense_dark_souls_ai.net neural network file not found\n");
        return EXIT_FAILURE;
    }
    defense_mind_input->mind = defense_mind;
    defense_mind_input->exit = false;
    HANDLE* defense_mind_thread = CreateThread(NULL, 0, DefenseMindProcess, NULL, 0, NULL);
    DefenseChoice = 0;

    attack_mind_input = malloc(sizeof(MindInput));
    struct fann* attack_mind = fann_create_from_file("Attack_dark_souls_ai.net");
    if (attack_mind == NULL){
        printf("Attack_dark_souls_ai.net neural network file not found\n");
        return EXIT_FAILURE;
    }
    attack_mind_input->mind = attack_mind;
    attack_mind_input->exit = false;
    HANDLE* attack_mind_thread = CreateThread(NULL, 0, AttackMindProcess, NULL, 0, NULL);
    AttackChoice = 0;

	//get current camera details to lock
	readCamera(&processHandle,memorybase);

	//set window focus
	HWND h = FindWindow(NULL, TEXT("DARK SOULS"));
	SetForegroundWindow(h);
	SetFocus(h);

	//TODO load vJoy driver(we ONLY want the driver loaded when program running)

	while (1){
		//TODO lock the camera
		//lockCamera(&processHandle);

		//read the data at these pointers, now that offsets have been added and we have a static address
		ReadPlayer(&Enemy, &processHandle);
		ReadPlayer(&Player, &processHandle);

        #if 0
		printf("Enemy : ");
		PrintPhantom(&Enemy);
		printf("Player: ");
		PrintPhantom(&Player);
        #endif

        //update neural network thread data
        defense_mind_input->input[0] = distance(&Player, &Enemy);
        defense_mind_input->input[1] = angleDeltaFromFront(&Player, &Enemy);
        defense_mind_input->input[2] = approachSpeed(&Player, &Enemy);

        defense_mind_input->input[0] = distance(&Player, &Enemy);
        defense_mind_input->input[1] = angleDeltaFromFront(&Player, &Enemy);
        defense_mind_input->input[2] = approachSpeed(&Player, &Enemy);


		// reset struct info
		iReport.wAxisX = MIDDLE;
		iReport.wAxisY = MIDDLE;
		iReport.wAxisZ = MIDDLE;//this is l2 and r2
		iReport.wAxisYRot = MIDDLE;
		iReport.wAxisXRot = MIDDLE;
		iReport.lButtons = 0x00000000;

		//basic logic initilization choice

		//logic has subroutine catches to ensure we dont interupt subroutines.
		//need a continue catch to ensure that each frame has the routine run, since internal logic does not have garunteed forced continue's
		/*
		ex: we started an attack subroutine frame 1.
		Frame 2 has the enemy attack us and trigger aboutToBeHit().
		We go to dodge(), but the ai's internal catch says we're already in attack subroutine, so dont start the dodge subroutine.
		Now the ai's normal logic would be finished, but we havent continued out attack subroutine.
		These subroutine checks ensures that it is continued.
		*/

		unsigned char attackImminent = aboutToBeHit(&Player, &Enemy);

		//defense mind makes choice to defend or not(ex backstab metagame decisions).
		//handles actually backstab checks, plus looks at info from obveous direct attacks from aboutToBeHit
        if (attackImminent == 2 || subroutine_states[0] || DefenseChoice){
			dodge(&Player, &Enemy, &iReport);
            DefenseChoice = false;//unset neural network desision
		}

		//this is definitly too overeager
		//attack mind make choice about IF to attack or not, and how to attack
        if (!attackImminent || subroutine_states[1] || AttackChoice){
            attack(&Player, &Enemy, &iReport);
            AttackChoice = false;//unset neural network desision
        }

		//send this struct to the driver (only 1 call for setting all controls, much faster)
		UpdateVJD(iInterface, (PVOID)&iReport);

		//SetForegroundWindow(h);
		//SetFocus(h);
	}

	RelinquishVJD(iInterface);
	CloseHandle(processHandle);
	return 0;
}