#include "Source.h"

#define _CRT_SECURE_NO_WARNINGS
#pragma warning( disable: 4305 )//ignore dataloss conversion from double to float

//vjoy settings/variables
#pragma comment( lib, "VJOYINTERFACE" )//load vjoy library
JOYSTICK_POSITION iReport;

//neural net and desicion making settings/variables
MindInput* defense_mind_input;
volatile char DefenseChoice = 0;
MindInput* attack_mind_input;
volatile unsigned char AttackChoice = 0;

InstinctDecision instinct_decision;

HANDLE processHandle;
ullong memorybase;

int SetupandLoad(){
    //memset to ensure we dont have unusual char attributes at starting
    memset(&Enemy, 0, sizeof(Character));
    memset(&Player, 0, sizeof(Character));
    //TODO temp hardcoding
    Enemy.weaponRange = 6;
    Player.weaponRange = 2.5;

    //get access to dark souls memory
    char * processName = "DARKSOULS.exe";
    //get the process id from the name
    int processId = GetProcessIdFromName(processName);
	if (processId == -1) {
		printf("Unable to find DarkSouls.exe\n");
		return EXIT_FAILURE;
	}
    //open the handle
    processHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, processId);
    //get the base address of the process and append all other addresses onto it
    memorybase = GetModuleBase(processId, processName);
    Enemy_base_add += memorybase;
    player_base_add += memorybase;

    ReadPointerEndAddresses(processHandle);

    //start gui
    guiStart();

    //get current camera details to lock
    readCamera(&processHandle, memorybase);

    //load neural network and threads
    int error = ReadyThreads();
    if (error){
        return error;
    }

    //TODO load vJoy driver(we ONLY want the driver loaded when program running)
    //want to use controller input, instead of keyboard, as analog stick is more precise movement
    int loadresult = loadvJoy(iInterface);
    if (loadresult != 0){
        return loadresult;
    }
    iReport.bDevice = (BYTE)iInterface;
    ResetVJoyController();

    //set window focus
    HWND h = FindWindow(NULL, TEXT("DARK SOULS"));
    SetForegroundWindow(h);
    SetFocus(h);

    return EXIT_SUCCESS;
}

#define DebuggingPacifyDef 0
#define DebuggingPacifyAtk 0

void MainLogicLoop(){
		//TODO lock the camera
		//lockCamera(&processHandle);

		//read the data at these pointers, now that offsets have been added and we have a static address
        ReadPlayer(&Enemy, processHandle, EnemyId);
        ReadPlayer(&Player, processHandle, PlayerId);

        //log distance in memory
        AppendDistance(distance(&Player, &Enemy));

        //start the neural network threads
        WakeThread(defense_mind_input);
        WakeThread(attack_mind_input);

        ResetVJoyController();

		//generate instinct decision
		instinct_decision.subroutine_id.attackid = AtkNoneId;
		instinct_decision.subroutine_id.defenseid = DefNoneId;
		InstinctDecisionMaking(&instinct_decision);

        WaitForThread(defense_mind_input);
        guiPrint(LocationDetection",1:Defense Neural Network detected %d, and Attack %d", DefenseChoice, AttackChoice);
#if DebuggingPacifyDef
        DefenseChoice = 0;
#endif
		if (instinct_decision.priority_decision == EnterDodgeSubroutine || inActiveDodgeSubroutine() || (DefenseChoice>0)){
			dodge(&iReport, &instinct_decision, DefenseChoice);
		}

        WaitForThread(attack_mind_input);
        guiPrint(LocationDetection",2:Attack Neural Network decided %d", AttackChoice);
#if DebuggingPacifyAtk
        AttackChoice = 0;
#endif
		if (inActiveAttackSubroutine() || (AttackChoice && DefenseChoice <= 0)){
			attack(&iReport, &instinct_decision, AttackChoice);
        }

        //unset neural network desisions
        DefenseChoice = 0;
        AttackChoice = 0;
        //handle subroutine safe exits
        SafelyExitSubroutines();

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