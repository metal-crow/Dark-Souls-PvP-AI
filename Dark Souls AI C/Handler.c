#include "Source.h"
#include <stdlib.h>

void BlackCrystalOut(){
    ResetVJoyController();
    iReport.bHats = 0x1;//down d pad
    UpdateVJD(iInterface, (PVOID)&iReport);
    Sleep(100);
    iReport.bHats = 0x0;
    iReport.lButtons = square;
    UpdateVJD(iInterface, (PVOID)&iReport);
    Sleep(100);
    iReport.bHats = 0x1;//down d pad again to go back to red sign
    iReport.lButtons = 0x0;
    UpdateVJD(iInterface, (PVOID)&iReport);
    Sleep(100);
    iReport.bHats = 0x0;
    UpdateVJD(iInterface, (PVOID)&iReport);
    Sleep(10000);//TODO time is however long it takes to black crystal
}

static bool RedSignDown = false;

void PutDownRedSign(){
    ResetVJoyController();
    iReport.lButtons = square;
    UpdateVJD(iInterface, (PVOID)&iReport);
    Sleep(100);
    iReport.lButtons = 0x0;
    UpdateVJD(iInterface, (PVOID)&iReport);
    RedSignDown = true;
}

static bool RereadPointerEndAddress = true;

int main(void){
    if (SetupandLoad()){
        return EXIT_FAILURE;
    }

    while (1){
        ReadProcessMemory(processHandle, (LPCVOID)(Player.visualStatus_address), &(Player.visualStatus), 4, 0);//this memory read isnt directly AI related

        if (RereadPointerEndAddress){
            ReadPointerEndAddresses(processHandle);
        }

        //if AI is a red phantom
        if (Player.visualStatus == 2){
            RedSignDown = false;
            RereadPointerEndAddress = false;
            //enemy player is fairly close
            if(distance(&Player, &Enemy) < 50){
                MainLogicLoop();
                //once we die
                if (Player.hp <= 0){
                    RereadPointerEndAddress = true;
                }
            }
            //if enemy player far away, black crystal out
            else{
                RereadPointerEndAddress = true;
                BlackCrystalOut();
            }
        }
        //if AI in host world, and red sign not down, put down red sign
        else if (Player.visualStatus == 0 && !RedSignDown){
            PutDownRedSign();
        }
    }

    Exit();
}