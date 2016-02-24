#define _CRT_SECURE_NO_WARNINGS

#include "Source.h"

volatile bool listening1 = true;

DWORD WINAPI ListentoContinue1(void* data) {
    printf("e to exit\n");
    while (listening1){
        char input = getchar();
        if (input == 'e'){//exit
            listening1 = false;
        }
    }
    return 0;
}

FILE* fpdef;

void DumpStaminaMem(){
    int* staminaarray = malloc(99872);
    ReadProcessMemory(processHandle, (LPCVOID)0x03B5D4A8, (LPVOID)staminaarray, 99872, 0);

    for (int i = 0; i < 99872 / 4; i += 8){
        int animType = staminaarray[i + 1];
        if (animType != 40 && animType != 50 && 
            animType != 240 && animType != 250 && 
            animType != 400 && 
            animType != 430 && animType != 440 && animType != 450 && animType != 490 && 
            animType != 505 && animType != 510 && animType != 515 && animType != 516 &&
            animType != 600)
        {
            fprintf(fpdef, "%d,%d,%d\n", staminaarray[i + 0], animType, staminaarray[i + 5]);
        }
    }
}

static int lastAid = 0;
static int curAid = 0;

void ReadWeaponTiming(){
    ReadPlayer(&Player, processHandle, PlayerId);
    unsigned char hurtbox = 0;
    ReadProcessMemory(processHandle, (LPCVOID)0x06D70AC7, &hurtbox, 1, 0);
    ReadProcessMemory(processHandle, (LPCVOID)Player.animationId_address, &curAid, 4, 0);
    float timer = 0;
    ReadProcessMemory(processHandle, (LPCVOID)0x0707E17C, &timer, 4, 0);

    if (hurtbox && lastAid != curAid){
        printf("%d %f\n", curAid, timer);
        lastAid = curAid;
    }
}

int mainTESTING(void)
{
    fpdef = fopen("E:/Code Workspace/Dark Souls AI C/out.txt", "w");

    SetupandLoad();

    HANDLE thread = CreateThread(NULL, 0, ListentoContinue1, NULL, 0, NULL);

    while (listening1){

    }

    fclose(fpdef);
    CloseHandle(processHandle);
    return EXIT_SUCCESS;
}