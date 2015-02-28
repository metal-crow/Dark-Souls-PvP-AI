#include "MemoryEdits.h"
#include "CharacterStruct.h"
#include "Interface.h"
#include "AIMethods.h"

#pragma comment( lib, "VJOYINTERFACE" )//load vjoy library
#define  _CRT_SECURE_NO_WARNINGS

int main(void){
	//initalize the phantom and player
	Character Enemy;
	memset(&Enemy, 0, sizeof(Character));
	Character Player;
	memset(&Player, 0, sizeof(Character));

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
	Enemy.r_weapon_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_r_weapon_offsets_length, Enemy_r_weapon_offsets);
	Enemy.l_weapon_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_l_weapon_offsets_length, Enemy_l_weapon_offsets);

	Player.location_x_address = FindPointerAddr(processHandle, player_base_add, Player_loc_x_offsets_length, Player_loc_x_offsets);
	Player.location_y_address = FindPointerAddr(processHandle, player_base_add, Player_loc_y_offsets_length, Player_loc_y_offsets);
	Player.rotation_address = FindPointerAddr(processHandle, player_base_add, Player_rotation_offsets_length, Player_rotation_offsets);
	Player.animation_address = FindPointerAddr(processHandle, player_base_add, Player_animation_offsets_length, Player_animation_offsets);
	Player.r_weapon_address = FindPointerAddr(processHandle, player_base_add, Player_r_weapon_offsets_length, Player_r_weapon_offsets);
	Player.l_weapon_address = FindPointerAddr(processHandle, player_base_add, Player_l_weapon_offsets_length, Player_l_weapon_offsets);

	//want to use controller input, instead of keyboard, as analog stick is more precise movement
	UINT iInterface = 1;								// Default target vJoy device
	BOOL ContinuousPOV = FALSE;						// Continuous POV hat (or 4-direction POV Hat)

	int loadresult = loadvJoy(iInterface);
	if (loadresult != 0){
		return loadresult;
	}

	JOYSTICK_POSITION iReport;
	iReport.bDevice = (BYTE)iInterface;

	//set window focus
	HWND h = FindWindow(NULL, TEXT("DARK SOULS"));
	SetForegroundWindow(h);
	SetFocus(h);

	//TODO load vJoy driver(we ONLY want the driver loaded when program running)

	while (1){
		//read the data at these pointers, now that offsets have been added and we have a static address
		ReadPlayer(&Enemy, &processHandle);
		ReadPlayer(&Player, &processHandle);

		printf("Enemy : ");
		PrintPhantom(&Enemy);
		printf("Player : ");
		PrintPhantom(&Player);

		// reset struct info
		iReport.wAxisX = MIDDLE;
		iReport.wAxisY = MIDDLE;
		iReport.wAxisZ = MIDDLE;//this is l2 and r2
		iReport.wAxisYRot = MIDDLE;
		iReport.wAxisXRot = MIDDLE;
		iReport.wAxisZRot = MIDDLE;
		iReport.lButtons = 0;

		//basic logic initilization choice
		if (aboutToBeHit(&Player, &Enemy)){
			dodge(&Player, &Enemy, &iReport);
		} else{
			attack(&Player, &Enemy, &iReport);
		}

		//send this struct to the driver (only 1 call for setting all controls, much faster)
		UpdateVJD(iInterface, (PVOID)&iReport);

		//SetForegroundWindow(h);
		//SetFocus(h);

		Sleep(20);
	}

	RelinquishVJD(iInterface);
	CloseHandle(processHandle);
	return 0;
}