#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <stdio.h>
#include "CharacterStruct.h"
#include <stdbool.h>
#include "MemoryEdits.h"
#include "Interface.h"

#pragma comment( lib, "VJOYINTERFACE" )//load vjoy library

typedef struct{
    int aid;
    float timer;
    unsigned char hurtbox;
}toRead;

toRead Enemy;

volatile bool listening1 = true;

DWORD WINAPI ListentoContinue1(void* data) {
    printf("e to exit");
    while (listening1){
        char input = getchar();
        if (input == 'e'){//exit
            listening1 = false;
        }
    }
    return 0;
}

#define aidADD 0x06FFA2C0
#define timerADD 0x0780249C
#define hurtboxADD 0x06FFA2E7

int mainHURTBOXTIMINGS(void)
{
    FILE* fpdef = fopen("E:/Code Workspace/Dark Souls AI C/hurtbox timings.txt", "a");

    unsigned char wait = 0;

    memset(&Enemy, 0, sizeof(toRead));

    //get access to dark souls memory
    char * processName = "DARKSOULS.exe";
    //get the process id from the name
    int processId = GetProcessIdFromName(processName);
    //open the handle
    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, processId);

    HANDLE thread = CreateThread(NULL, 0, ListentoContinue1, NULL, 0, NULL);

    int lastaid = -1;

    while (listening1){
        ReadProcessMemory(processHandle, (LPCVOID)(aidADD), &(Enemy.aid), 4, 0);
        ReadProcessMemory(processHandle, (LPCVOID)(timerADD), &(Enemy.timer), 4, 0);
        ReadProcessMemory(processHandle, (LPCVOID)(hurtboxADD), &(Enemy.hurtbox), 1, 0);
        
        if (Enemy.hurtbox && lastaid != Enemy.aid){
            fprintf(fpdef, "%d %f\n", Enemy.aid, Enemy.timer);
            printf("%d calc:%f act:%f\n", Enemy.aid, Enemy.timer);
            lastaid = Enemy.aid;
        }

    }

    fclose(fpdef);
    CloseHandle(processHandle);
    return EXIT_SUCCESS;
}