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

toRead EnemyHurtboxTimings;

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

#define aidADD 0x0ABC2A84
#define timerADD 0x0ABC2B1C
#define hurtboxADD 0x0AC85E07

int mainHURTBOXTIMINGS(void)
{
    FILE* fpdef = fopen("E:/Code Workspace/Dark Souls AI C/hurtbox timings.txt", "a");

    unsigned char wait = 0;

    memset(&EnemyHurtboxTimings, 0, sizeof(toRead));

    //get access to dark souls memory
    char * processName = "DARKSOULS.exe";
    //get the process id from the name
    int processId = GetProcessIdFromName(processName);
    //open the handle
    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, processId);

    HANDLE thread = CreateThread(NULL, 0, ListentoContinue1, NULL, 0, NULL);

    int lastaid = -1;

    while (listening1){
        ReadProcessMemory(processHandle, (LPCVOID)(aidADD), &(EnemyHurtboxTimings.aid), 4, 0);
        ReadProcessMemory(processHandle, (LPCVOID)(timerADD), &(EnemyHurtboxTimings.timer), 4, 0);
        ReadProcessMemory(processHandle, (LPCVOID)(hurtboxADD), &(EnemyHurtboxTimings.hurtbox), 1, 0);
        
        if (EnemyHurtboxTimings.hurtbox && lastaid != EnemyHurtboxTimings.aid){
            fprintf(fpdef, "%d %f\n", EnemyHurtboxTimings.aid, EnemyHurtboxTimings.timer);
            printf("%d time:%f\n", EnemyHurtboxTimings.aid, EnemyHurtboxTimings.timer);
            lastaid = EnemyHurtboxTimings.aid;
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

int mainTESTING(){
    memset(&EnemyHurtboxTimings, 0, sizeof(toRead));

    UINT iInterface = 1;								// Default target vJoy device
    int loadresult = loadvJoy(iInterface);
    if (loadresult != 0){
        return loadresult;
    }
    JOYSTICK_POSITION iReport;
    iReport.bDevice = (BYTE)iInterface;

    guiStart();

    //get access to dark souls memory
    char * processName = "DARKSOULS.exe";
    //get the process id from the name
    int processId = GetProcessIdFromName(processName);
    //open the handle
    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, processId);

    while (true){
        ReadProcessMemory(processHandle, (LPCVOID)(aidADD), &(EnemyHurtboxTimings.aid), 4, 0);
        ReadProcessMemory(processHandle, (LPCVOID)(timerADD), &(EnemyHurtboxTimings.timer), 4, 0);
        ReadProcessMemory(processHandle, (LPCVOID)(hurtboxADD), &(EnemyHurtboxTimings.hurtbox), 1, 0);

        // reset struct info
        iReport.wAxisX = MIDDLE;
        iReport.wAxisY = MIDDLE;
        iReport.wAxisZ = MIDDLE;//this is l2 and r2
        iReport.wAxisYRot = MIDDLE;
        iReport.wAxisXRot = MIDDLE;
        iReport.lButtons = 0x0;

        float dodgeTimer = dodgeTimings(EnemyHurtboxTimings.aid);
        float timeDelta = dodgeTimer - EnemyHurtboxTimings.timer;
        guiPrint(LocationState",1:Delta:%f",timeDelta);

        if (timeDelta <= 0.17 && timeDelta>-0.17){
            iReport.wAxisY = YTOP;
            iReport.lButtons = circle;
            guiPrint(LocationState",0:circle + forward");
        } else{
            guiPrint(LocationState",0:");
        }

        UpdateVJD(iInterface, (PVOID)&iReport);
    }
}