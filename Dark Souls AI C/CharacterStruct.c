#include "CharacterStruct.h"

void PrintPhantom(Character * c){
	printf("X: %f Y: %f Rotation: %f Animation id: %i\n", c->loc_x, c->loc_y, c->rotation, c->animation_id);
}

void ReadPlayer(Character * c, HANDLE * processHandle){
	HANDLE processHandle_nonPoint = *processHandle;
	ReadProcessMemory(processHandle_nonPoint, (LPCVOID)(c->location_x_address), &(c->loc_x), 4, 0);
	ReadProcessMemory(processHandle_nonPoint, (LPCVOID)(c->location_y_address), &(c->loc_y), 4, 0);
	ReadProcessMemory(processHandle_nonPoint, (LPCVOID)(c->animation_address), &(c->rotation), 4, 0);
	ReadProcessMemory(processHandle_nonPoint, (LPCVOID)(c->animation_address), &(c->animation_id), 2, 0);
	ReadProcessMemory(processHandle_nonPoint, (LPCVOID)(c->r_weapon_address), &(c->r_weapon_id), 4, 0);
	ReadProcessMemory(processHandle_nonPoint, (LPCVOID)(c->l_weapon_address), &(c->l_weapon_id), 4, 0);
}