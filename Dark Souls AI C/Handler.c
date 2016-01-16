#include "Source.h"
#include <stdlib.h>

void BlackCrystalOut(){
    iReport.bHats = 0x1;//down d pad
    Sleep(100);
    iReport.bHats = 0x0;
    iReport.lButtons = square;
    Sleep(100);
    iReport.bHats = 0x1;//down d pad again to go back to red sign
    iReport.lButtons = 0x0;
    Sleep(100);
    iReport.bHats = 0x0;
    Sleep(10000);//TODO time is however long it takes to black crystal
}

void PutDownRedSign(){
    iReport.lButtons = square;
    Sleep(100);
    iReport.lButtons = 0x0;
}

int main(void){
    if (SetupandLoad()){
        return EXIT_FAILURE;
    }

    while (1){
        ReadProcessMemory(processHandle, (LPCVOID)(Player.visualStatus_address), &(Player.visualStatus), 4, 0);//this memory read isnt directly AI related

        //if AI is a red phantom and enemy player is fairly close
        if (Player.visualStatus == 2 && distance(&Player, &Enemy) < 100){
            MainLogicLoop();
        }
        //if enemy player far away, black crystal out
        else if (Player.visualStatus == 2 && distance(&Player, &Enemy) > 100){
            BlackCrystalOut();
        }
        //if AI in host world, and TODO red sign not down, put down red sign
        else if (Player.visualStatus == 0){
            PutDownRedSign();
        }
    }

    Exit();
}