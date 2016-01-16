#include "Source.h"

#define _CRT_SECURE_NO_WARNINGS
#pragma warning( disable: 4305 )//ignore dataloss conversion from double to float

//vjoy settings/variables
#pragma comment( lib, "VJOYINTERFACE" )//load vjoy library
#define iInterface 1// Default target vJoy device
JOYSTICK_POSITION iReport;

//neural net and desicion making settings/variables
MindInput* defense_mind_input;
volatile char DefenseChoice = 0;
MindInput* attack_mind_input;
volatile unsigned char AttackChoice = 0;

HANDLE processHandle;

int SetupandLoad(){
    //memset to ensure we dont have unusual char attributes at starting
    memset(&Enemy, 0, sizeof(Character));
    Enemy.weaponRange = 6;//TODO temp hardcoding
    memset(&Player, 0, sizeof(Character));
    Player.weaponRange = 2.5;

    //get access to dark souls memory
    char * processName = "DARKSOULS.exe";
    //get the process id from the name
    int processId = GetProcessIdFromName(processName);
    //open the handle
    processHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, processId);
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
    Enemy.animationType_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_animationType_offsets_length, Enemy_animationType_offsets);
    Enemy.hp_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_hp_offsets_length, Enemy_hp_offsets);
    Enemy.stamina_address = 0;
    Enemy.r_weapon_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_r_weapon_offsets_length, Enemy_r_weapon_offsets);
    Enemy.l_weapon_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_l_weapon_offsets_length, Enemy_l_weapon_offsets);
    Enemy.animationTimer_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_animationTimer_offsets_length, Enemy_animationTimer_offsets);
    Enemy.animationTimer2_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_animationTimer2_offsets_length, Enemy_animationTimer2_offsets);
    Enemy.animationId_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_animationID_offsets_length, Enemy_animationID_offsets);
    Enemy.animationId2_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_animationID2_offsets_length, Enemy_animationID2_offsets);
    Enemy.hurtboxActive_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_hurtboxActive_offsets_length, Enemy_hurtboxActive_offsets);
    Enemy.readyState_address = 0;
    Enemy.velocity_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_velocity_offsets_length, Enemy_velocity_offsets);
    Enemy.locked_on_address = 0;
    Enemy.twoHanding_address = 0;
    Enemy.visualStatus_address = 0;

    Player.location_x_address = FindPointerAddr(processHandle, player_base_add, Player_loc_x_offsets_length, Player_loc_x_offsets);
    Player.location_y_address = FindPointerAddr(processHandle, player_base_add, Player_loc_y_offsets_length, Player_loc_y_offsets);
    Player.rotation_address = FindPointerAddr(processHandle, player_base_add, Player_rotation_offsets_length, Player_rotation_offsets);
    Player.animationType_address = FindPointerAddr(processHandle, player_base_add, Player_animationType_offsets_length, Player_animationType_offsets);
    Player.hp_address = FindPointerAddr(processHandle, player_base_add, Player_hp_offsets_length, Player_hp_offsets);
    Player.stamina_address = FindPointerAddr(processHandle, player_base_add, Player_stamina_offsets_length, Player_stamina_offsets);
    Player.r_weapon_address = FindPointerAddr(processHandle, player_base_add, Player_r_weapon_offsets_length, Player_r_weapon_offsets);
    Player.l_weapon_address = FindPointerAddr(processHandle, player_base_add, Player_l_weapon_offsets_length, Player_l_weapon_offsets);
    Player.animationTimer_address = FindPointerAddr(processHandle, player_base_add, Player_animationTimer_offsets_length, Player_animationTimer_offsets);
    Player.animationTimer2_address = FindPointerAddr(processHandle, player_base_add, Player_animationTimer2_offsets_length, Player_animationTimer2_offsets);
    Player.animationId_address = FindPointerAddr(processHandle, player_base_add, Player_animationID_offsets_length, Player_animationID_offsets);
    Player.animationId2_address = FindPointerAddr(processHandle, player_base_add, Player_animationID2_offsets_length, Player_animationID2_offsets);
    Player.hurtboxActive_address = 0;
    Player.readyState_address = FindPointerAddr(processHandle, player_base_add, Player_readyState_offsets_length, Player_readyState_offsets);
    Player.velocity_address = 0;
    Player.locked_on_address = FindPointerAddr(processHandle, player_base_add, Player_Lock_on_offsets_length, Player_Lock_on_offsets);
    Player.twoHanding_address = FindPointerAddr(processHandle, player_base_add, Player_twohanding_offsets_length, Player_twohanding_offsets);
    Player.visualStatus_address = FindPointerAddr(processHandle, player_base_add, Player_visual_offsets_length, Player_visual_offsets);

    //want to use controller input, instead of keyboard, as analog stick is more precise movement
    int loadresult = loadvJoy(iInterface);
    if (loadresult != 0){
        return loadresult;
    }
    iReport.bDevice = (BYTE)iInterface;

    //load neural network and threads
    int error = ReadyThreads();
    if (error){
        return error;
    }

    //start gui
    guiStart();

    //get current camera details to lock
    readCamera(&processHandle, memorybase);

    //set window focus
    HWND h = FindWindow(NULL, TEXT("DARK SOULS"));
    SetForegroundWindow(h);
    SetFocus(h);

    //TODO load vJoy driver(we ONLY want the driver loaded when program running)

    return EXIT_SUCCESS;
}

#define DebuggingPacify 0

void MainLogicLoop(){
		//TODO lock the camera
		//lockCamera(&processHandle);

		//read the data at these pointers, now that offsets have been added and we have a static address
        ReadPlayer(&Enemy, &processHandle, LocationMemoryEnemy);
        ReadPlayer(&Player, &processHandle, LocationMemoryPlayer);

        //start the neural network threads
        WakeThread(defense_mind_input);
        WakeThread(attack_mind_input);

		// reset struct info
		iReport.wAxisX = MIDDLE;
		iReport.wAxisY = MIDDLE;
		iReport.wAxisZ = MIDDLE;//this is l2 and r2
		iReport.wAxisYRot = MIDDLE;
		iReport.wAxisXRot = MIDDLE;
        iReport.lButtons = 0x0;
        iReport.bHats = 0x0;//d-pad

		//begin reading enemy state, and handle w logic and subroutines
        char attackImminent = EnemyStateProcessing(&Player, &Enemy);

        WaitForThread(defense_mind_input);
        guiPrint(LocationDetection",1:Defense Neural Network detected %d, and Attack %d", DefenseChoice, AttackChoice);
#if DebuggingPacify
        DefenseChoice = 0;
#endif
		//defense mind makes choice to defend or not(ex backstab metagame decisions).
		//handles actually backstab checks, plus looks at info from obveous direct attacks from aboutToBeHit
        if (attackImminent == ImminentHit || inActiveDodgeSubroutine() || (DefenseChoice>0)){
            dodge(&Player, &Enemy, &iReport, attackImminent, DefenseChoice);
            DefenseChoice = 0;//unset neural network desision
		}

        WaitForThread(attack_mind_input);
        guiPrint(LocationDetection",2:Attack Neural Network decided %d", AttackChoice);
#if DebuggingPacify
        AttackChoice = 0;
#endif
		//attack mind make choice about IF to attack or not, and how to attack
        //enter when we either have a Attack neural net action or a attackImminent action
        if (inActiveAttackSubroutine() || attackImminent != ImminentHit || AttackChoice){
            attack(&Player, &Enemy, &iReport, attackImminent, AttackChoice);
            AttackChoice = 0;//unset neural network desision
        }

        guiPrint(LocationDetection",5:Current Subroutine States ={%d,%d,%d,%d}", subroutine_states[0], subroutine_states[1], subroutine_states[2], subroutine_states[3]);

		//send this struct to the driver (only 1 call for setting all controls, much faster)
        guiPrint(LocationJoystick",0:AxisX:%d\nAxisY:%d\nButtons:0x%x", iReport.wAxisX, iReport.wAxisY, iReport.lButtons);
		UpdateVJD(iInterface, (PVOID)&iReport);

		//SetForegroundWindow(h);
		//SetFocus(h);
}

void Exit(){
	RelinquishVJD(iInterface);
    defense_mind_input->exit = true;
    attack_mind_input->exit = true;
	CloseHandle(processHandle);
    guiClose();
}