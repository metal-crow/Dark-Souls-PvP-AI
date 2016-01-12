#include "Source.h"
#include <stdlib.h>

int main(void){
    if (SetupandLoad()){
        return EXIT_FAILURE;
    }

    while (1){
        MainLogicLoop();
    }

    Exit();
}