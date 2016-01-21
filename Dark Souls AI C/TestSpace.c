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
    ReadProcessMemory(processHandle, 0x03C2D4A8, (LPVOID)staminaarray, 99872, 0);

    int row = 0;
    for (int i = 0; i < 99872 / 4; i += 8){
        if (row != 4 && row != 5 && row != 12 && row != 13 && row != 16 && row != 19 && row != 20 && row != 21 && row != 22 && row != 24 && row != 25 && row != 26 && row != 27 && row != 32){
            fprintf(fpdef, "%d,%d,%d\n", staminaarray[i + 0], staminaarray[i + 1], staminaarray[i + 5]);
        }
        row++;
        if (row == 33){
            row = 0;
        }
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