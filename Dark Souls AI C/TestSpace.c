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

int mainTESTING(void)
{
    FILE* fpdef = fopen("E:/Code Workspace/Dark Souls AI C/out.txt", "w");

    SetupandLoad();

    HANDLE thread = CreateThread(NULL, 0, ListentoContinue1, NULL, 0, NULL);

    while (listening1){

    }

    fclose(fpdef);
    CloseHandle(processHandle);
    return EXIT_SUCCESS;
}