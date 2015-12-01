#define _CRT_SECURE_NO_WARNINGS

#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
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
    printf("e to exit\n");
    while (listening1){
        char input = getchar();
        if (input == 'e'){//exit
            listening1 = false;
        }
    }
    return 0;
}

#define aidADD 0x06829860
#define timerADD 0x070297DC
#define hurtboxADD 0x0B6C4C24

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
        
        if (Enemy.hurtbox==1 && lastaid != Enemy.aid){
            fprintf(fpdef, "%d %f\n", Enemy.aid, Enemy.timer);
            printf("%d time:%f\n", Enemy.aid, Enemy.timer);
            lastaid = Enemy.aid;
        }

    }

    fclose(fpdef);
    CloseHandle(processHandle);
    return EXIT_SUCCESS;
}

int mainWRITINGMEMORY(void){
    char * processName = "DARKSOULS.exe";
    //get the process id from the name
    int processId = GetProcessIdFromName(processName);
    //open the handle
    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, processId);
    unsigned char writing = 100;
    printf("%d\n", WriteProcessMemory(processHandle, (LPVOID)(0x06CD94B4), &writing, 4, 0));
    printf("%lu\n", GetLastError());
    return 0;
}