#include "AnimationMappings.h"

/*determine what type of action the animation id is.
i cant seem to find a pattern in the ids, so this is just a big switch statement
ranged attacks use a different format: they have a specific animation for windup,recover, and hurtbox creation; while others rely on a subanimation id to determine windup, hurtbox, and recovery.
because of this, have to specify if we need to look at subanimation
0 is not attack animation, 1 is windup to attack, 2 is attack id but must check subanimation(hurtbox not instantly generated), 3 is hurtbox is created*/
unsigned char isAttackAnimation(unsigned char animation_id){
    switch (animation_id){
        //nothing
        //case 0:
        //    return 0;
        //could not use
        //case 21:
        //    return 0;
        //1h rolling attack
        case 41:
            return 2;
        //1h r1
        case 46:
            return 2;
        //1h r1 swing 2
        case 48:
            return 2;
        //1h r1 swing 3
        case 49:
            return 2;
        //jumping 1 hand
        case 53:
            return 2;
        //left hand attack
        case 55:
            return 2;
        //backstep attack 1h
        case 59:
            return 2;
        //backstep attack 2h
        case 60:
            return 2;
        //crossbow/bow windup 2h
        case 65:
            return 0;
        //bow hold
        case 66:
            return 0;
        //crossbow/bow attack 2h
        case 67:
            return 3;
        //crossbow recover 2h
        //case 68:
        //    return 0;
        //crossbow windup 1h
        case 69:
            return 0;
        //crossbow attack 1h
        case 70:
            return 3;
        //crossbow recover 1h
        //case 71:
        //    return 0;
        //shield raise
        //case 76:
        //    return 0;
        //shield up
        //case 77:
        //    return 0;
        //shied lower
        //case 78:
        //    return 0;
        //parry
        case 86:
            return 1;//not a windup, but pretend it is to prevent attack
        //1h r2
        case 89:
            return 2;
        //rolling attack 2h
        case 103:
            return 2;
        //2h r1
        case 107:
            return 2;
        //2h r1 bounce back
        //case 108:
        //    return 0;
        //2h r1 combo
        case 109:
            return 2;
        //2h r1 combo
        case 110:
            return 2;
        //jumping 2 hand
        case 113:
            return 2;
        //2h r2
        case 115:
            return 2;
        //firestorm windup
        case 163:
            return 0;//is a windup, but want to attack during it
        //firstorm
        case 164:
            return 3;
        //flame windup
        case 167:
            return 1;
        //flame attack
        case 168:
            return 3;
        //pyro ball windup
        case 173:
            return 1;
        //pyro ball throw
        case 174:
            return 3;
        //1 hand weapon
        //case 185:
        //    return 0;
        //1 hand weapon
        //case 188:
        //    return 0;
        //2 hand weapon
        //case 199:
        //    return 0;
        //2 hand weapon
        //case 203:
        //    return 0;
        //weapon switch r
        //case 209:
        //    return 0;
        //weapon switch r
        //case 213:
        //    return 0;
        //weapon switch l
        //case 217:
        //    return 0;
        //weapon switch l
        //case 221:
        //    return 0;
        //backstab
        //case 225:
        //    return 0;
        default:
            //printf("unknown animation id\n");
            return 0;
    }
}


unsigned char isDodgeAnimation(unsigned char animation_id){
    switch (animation_id){
        //1 hand roll
        case 32:
            return 1;
        //backstep 1h
        case 38:
            return 1;
        //2 hand roll
        case 94:
            return 1;
        //backstep 2h
        case 100:
            return 1;
        default:
            return 0;
    }
}