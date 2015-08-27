#define _CRT_SECURE_NO_WARNINGS

#include "MemoryEdits.h"
#include "CharacterStruct.h"
#include "Interface.h"
#include "AIMethods.h"
#include "SubRoutines.h"
#include "MindRoutines.h"

#include "fann.h"

#pragma comment( lib, "VJOYINTERFACE" )//load vjoy library

//initalize the phantom and player
Character Enemy;
Character Player;

//intialize extern variables for neural net
MindInput* defense_mind_input;
volatile unsigned char DefenseChoice = 0;
MindInput* attack_mind_input;
volatile unsigned char AttackChoice = 0;

int main(void){
    //freopen("output.txt", "w", stdout);

	//memset to ensure we dont have unusual char attributes at starting
	memset(&Enemy, 0, sizeof(Character));
	Enemy.weaponRange = 3;//TODO temp hardcoding
	memset(&Player, 0, sizeof(Character));
	Player.weaponRange = 3;

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

    //TODO THESE HAVE TO BE REREAD, AS THE END ADDRESS CAN CHANGE
    //MOVE TO SETUP METHOD in CharacterStruct
	//add the pointer offsets to the address. This can be slow because its startup only
	Enemy.location_x_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_loc_x_offsets_length, Enemy_loc_x_offsets);
	Enemy.location_y_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_loc_y_offsets_length, Enemy_loc_y_offsets);
	Enemy.rotation_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_rotation_offsets_length, Enemy_rotation_offsets);
	Enemy.animation_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_animation_offsets_length, Enemy_animation_offsets);
	Enemy.hp_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_hp_offsets_length, Enemy_hp_offsets);
    Enemy.stamina_address = 0;
	Enemy.r_weapon_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_r_weapon_offsets_length, Enemy_r_weapon_offsets);
	Enemy.l_weapon_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_l_weapon_offsets_length, Enemy_l_weapon_offsets);
	Enemy.subanimation_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_subanimation_offsets_length, Enemy_subanimation_offsets);
    Enemy.hurtboxActive_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_hurtboxActive_offsets_length, Enemy_hurtboxActive_offsets);
    Enemy.windupClose_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_windupClose_offsets_length, Enemy_windupClose_offsets);
    Enemy.velocity_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_velocity_offsets_length, Enemy_velocity_offsets);

	Player.location_x_address = FindPointerAddr(processHandle, player_base_add, Player_loc_x_offsets_length, Player_loc_x_offsets);
	Player.location_y_address = FindPointerAddr(processHandle, player_base_add, Player_loc_y_offsets_length, Player_loc_y_offsets);
	Player.rotation_address = FindPointerAddr(processHandle, player_base_add, Player_rotation_offsets_length, Player_rotation_offsets);
	Player.animation_address = FindPointerAddr(processHandle, player_base_add, Player_animation_offsets_length, Player_animation_offsets);
	Player.hp_address = FindPointerAddr(processHandle, player_base_add, Player_hp_offsets_length, Player_hp_offsets);
    Player.stamina_address = FindPointerAddr(processHandle, player_base_add, Player_stamina_offsets_length, Player_stamina_offsets);
	Player.r_weapon_address = FindPointerAddr(processHandle, player_base_add, Player_r_weapon_offsets_length, Player_r_weapon_offsets);
	Player.l_weapon_address = FindPointerAddr(processHandle, player_base_add, Player_l_weapon_offsets_length, Player_l_weapon_offsets);
	Player.subanimation_address = FindPointerAddr(processHandle, player_base_add, Player_subanimation_offsets_length, Player_subanimation_offsets);
    Player.hurtboxActive_address = 0;
    Player.windupClose_address = FindPointerAddr(processHandle, player_base_add, Player_windupClose_offsets_length, Player_windupClose_offsets);
    Player.velocity_address = 0;

	//want to use controller input, instead of keyboard, as analog stick is more precise movement
	UINT iInterface = 1;								// Default target vJoy device
	int loadresult = loadvJoy(iInterface);
	if (loadresult != 0){
		return loadresult;
	}
	JOYSTICK_POSITION iReport;
	iReport.bDevice = (BYTE)iInterface;

    //load neural network and threads
    int error = ReadyThreads();
    if (error){
        return error;
    }

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

        //Defense input: player distance, angle delta, velocity, rotation delta,
        //get input and scale from -1 to 1 
        float distanceInput = distance(&Player, &Enemy);
        //min:0.3 max:5
        defense_mind_input->input[0] = 2 * (distanceInput - 0.3) / (5 - 0.3) - 1;

        float angleDeltaInput = angleDeltaFromFront(&Player, &Enemy);
        //min:0 max:1.6
        defense_mind_input->input[1] = 2 * (angleDeltaInput) / (1.6) - 1;

        //min:-0.18 max:-0.04
        defense_mind_input->input[2] = 2 * (Enemy.velocity - -0.18) / (-0.04 - -0.18) - 1;

        float rotationDeltaInput = rotationDifferenceFromSelf(&Player, &Enemy);
        //min:0 max:3.8
        defense_mind_input->input[3] = 2 * (rotationDeltaInput) / (3.8) - 1;

        WakeThread(defense_mind_input);

        //Attack input: player distance
        attack_mind_input->input[0] = defense_mind_input->input[0];
        attack_mind_input->input[3] = (float)Player.stamina;//send over for post check, neural network doesnt need to worry about, we can handle after

        WakeThread(attack_mind_input);

		// reset struct info
		iReport.wAxisX = MIDDLE;
		iReport.wAxisY = MIDDLE;
		iReport.wAxisZ = MIDDLE;//this is l2 and r2
		iReport.wAxisYRot = MIDDLE;
		iReport.wAxisXRot = MIDDLE;
        iReport.lButtons = 0x0;

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

        WaitForThread(defense_mind_input);
        //DefenseChoice = 0;
        //printf("defense %d\n",DefenseChoice);

		//defense mind makes choice to defend or not(ex backstab metagame decisions).
		//handles actually backstab checks, plus looks at info from obveous direct attacks from aboutToBeHit
        if (attackImminent == 2 || inActiveDodgeSubroutine() || DefenseChoice){
            dodge(&Player, &Enemy, &iReport, DefenseChoice);
            DefenseChoice = 0;//unset neural network desision
		}

        WaitForThread(attack_mind_input);
        //AttackChoice = 0;
        //printf("attack %d\n", AttackChoice);

		//attack mind make choice about IF to attack or not, and how to attack
        if (inActiveAttackSubroutine() || (!attackImminent && AttackChoice)){
            attack(&Player, &Enemy, &iReport, AttackChoice);
            AttackChoice = 0;//unset neural network desision
        }

		//send this struct to the driver (only 1 call for setting all controls, much faster)
		UpdateVJD(iInterface, (PVOID)&iReport);

		//SetForegroundWindow(h);
		//SetFocus(h);
	}

	RelinquishVJD(iInterface);
    defense_mind_input->exit = true;
    attack_mind_input->exit = true;
	CloseHandle(processHandle);
	return EXIT_SUCCESS;
}