#include "WeaponData.h"

//this can theoreticly be read in memory, but thats slow
int BaseStaminaUseForWeapon(unsigned int weaponId){
    if ((weaponId >= 100000 && weaponId < 200000) || 
        (weaponId >= 9011000 && weaponId <= 9011005)){
        return 14;
    }
    else if ((weaponId >= 200000 && weaponId < 212000) ||
        (weaponId >= 400000 && weaponId < 40400) ||
        (weaponId >= 400000 && weaponId < 40400) ||
        (weaponId >= 9010000 && weaponId < 9020000) ||
        (weaponId >= 406000 && weaponId < 407000) ||
        (weaponId >= 600000 && weaponId < 605000)){
        return 20;
    }
    else if (weaponId >= 500000 && weaponId < 504000){
        return 24;
    }
    else if ((weaponId >= 700000 && weaponId < 706000) ||
        (weaponId >= 800000 && weaponId < 813000) ||
        (weaponId >= 1000000 && weaponId < 1007000) ||
        (weaponId >= 1051000 && weaponId < 1053000)){
        return 25;
    }
    else if ((weaponId >= 300000 && weaponId < 315000) ||
        (weaponId >= 9016000 && weaponId < 9030000) ||
        (weaponId >= 450000 && weaponId < 454000) ||
        (weaponId >= 1050000 && weaponId < 1152000)){
        return 30;
    }
    else if ((weaponId >= 750000 && weaponId < 754000) ||
        (weaponId >= 9015000 && weaponId < 9016000) ||
        (weaponId >= 850000 && weaponId < 857000)){
        return 35;
    }
    else if (weaponId >= 350000 && weaponId < 356000){
        return 45;
    }
}