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

#define aidADD 0x0ABC2A84
#define timerADD 0x0ABC2B1C
#define hurtboxADD 0x0AC85E07

int mainTESTING(void)
{
    FILE* fpdef = fopen("E:/Code Workspace/Dark Souls AI C/hurtbox timings.txt", "a");

    SetupandLoad();

    HANDLE thread = CreateThread(NULL, 0, ListentoContinue1, NULL, 0, NULL);

    bool hasReset = true;

    while (listening1){
        ReadPlayer(&Player, &processHandle, LocationMemoryPlayer);

        /*if (EnemyHurtboxTimings.hurtbox && lastaid != EnemyHurtboxTimings.aid){
            fprintf(fpdef, "%d %f\n", EnemyHurtboxTimings.aid, EnemyHurtboxTimings.timer);
            printf("%d time:%f\n", EnemyHurtboxTimings.aid, EnemyHurtboxTimings.timer);
            lastaid = EnemyHurtboxTimings.aid;
        }*/
        if (isAttackAnimation(Player.animationType_id) && hasReset && Player.stamina < 192){
            fprintf(fpdef, "%d %d\n", Player.r_weapon_id, 192 - Player.stamina);
            printf("%d %d\n", Player.r_weapon_id, 192 - Player.stamina);
            hasReset = false;
        }

        if (Player.subanimation == SubanimationNeutral){
            hasReset = true;
        }
    }

    fclose(fpdef);
    CloseHandle(processHandle);
    return EXIT_SUCCESS;
}