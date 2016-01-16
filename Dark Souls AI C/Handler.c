#include "Source.h"
#include <stdlib.h>

void BlackCrystalOut(){
    iReport.bHats = 0x1;//down d pad
    Sleep(100);
    iReport.bHats = 0x0;
    iReport.lButtons = square;
    Sleep(100);
    iReport.lButtons = 0x0;
    Sleep(1000);
}

int main(void){
    if (SetupandLoad()){
        return EXIT_FAILURE;
    }

    while (1){
        //if AI is a red phantom and enemy player is fairly close
        if (Player.visualStatus == 2 && distance(&Player, &Enemy) < 100){
            MainLogicLoop();
        }
        //if enemy player far away, black crystal out
        else if (distance(&Player, &Enemy) < 100){
            BlackCrystalOut();
        }
        //if AI in host world, and red sign not down, put down red sign

    }

    Exit();
}