#include "Source.h"

int main(void){
    SetupandLoad();

    while (1){
        MainLogicLoop();
    }

    Exit();
}