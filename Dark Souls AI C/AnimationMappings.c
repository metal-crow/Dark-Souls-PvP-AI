#include "AnimationMappings.h"

#pragma warning( disable: 4305 )//disable truncation from double to float

/*determine what type of action the animation id is.
i cant seem to find a pattern in the ids, so this is just a big switch statement
ranged attacks use a different format: they have a specific animation for windup,recover, and hurtbox creation; while others rely on a subanimation id to determine windup, hurtbox, and recovery.
because of this, have to specify if we need to look at subanimation
0 is not attack animation
1 is windup to attack
2 is attack id but must check subanimation(hurtbox not instantly generated)
3 is hurtbox is created
4 is same as 2 but override behind enemy safety
5 is only used with character struct for animation combination
*/
unsigned char isAttackAnimation(unsigned short animationType_id){
    switch (animationType_id){
        case DragonBreathFire:
            return 2;
		case DragonRoar:
			return 2;
        case RollingAttack_1H:
            return 2;
        case ShieldPoke:
            return 2;
        case R1_1H:
            return 2;
        case R1_1H_Combo1:
            return 2;
        case R1_1H_Combo2:
            return 2;
        case Kick_1H:
            return 2;
        case Jumping_1H:
            return 2;
        case LeftHandAttack:
            return 2;
        case Backstep_Attack_1H:
            return 2;
        case Backstep_Attack_2H:
            return 2;
        case Bow_Release_2H:
            return 3;
        case Crossbow_Release_1H:
            return 3;
        case R1_1H_into_R2:
            return 2;
        case R2_1H:
            return 2;
        case R2_1H_Combo1:
            return 2;
        case RollingAttack_2H:
            return 2;
        case R1_2H:
            return 2;
        case R1_2H_just2Handed:
            return 2;
        case R1_2H_Combo1:
            return 2;
        case R1_2H_Combo2:
            return 2;
        case Kick_2H:
            return 2;
        case Jumping_2H:
            return 2;
        case R1_2H_into_R2:
            return 2;
        case R2_2H:
            return 2;
        case R2_2H_Combo1:
            return 2;
        case FireSurge_Cast_LH:
            return 2;
        case FireSurge_Cast_RH:
            return 2;
        case Miricle_Projectile_Cast:
            return 2;
        case Miricle_AOE_Windup:
            return 4;
        case Miricle_AOE_Cast:
            return 5;
        case Miricle_Throw_Cast:
            return 3;
        case Miricle_Ground_Cast:
            return 3;
        case Magic_Cast_Cast:
            return 3;
        case FireWhip_Cast:
            return 2;
        case FireStorm_Cast:
            return 3;
        case Combustion_Windup:
            return 2;
        case Combustion_Cast:
            return 5;
        case FireBall_Windup:
            return 1;
        case FireBall_Cast:
            return 3;
        default:
            return 0;
    }
}


unsigned char isDodgeAnimation(unsigned short animationType_id){
    switch (animationType_id){
        case Roll_1H:
            return 1;
        case RollBackwards_1H:
            return 1;
        case RollRight_1H:
            return 1;
        case RollLeft_1H:
            return 1;
        case Backstep_1H:
            return 1;
        case Roll_2H:
            return 1;
        case RollBackwards_2H:
            return 1;
        case RollRight_2H:
            return 1;
        case RollLeft_2H:
            return 1;
        case Backstep_2H:
            return 1;
        default:
            return 0;
    }
}

unsigned char isVulnerableAnimation(int animation_id){
    switch (animation_id){
        //shield break heavy
        case 164:
            return 1;
        //shield break medium
        case 162:
            return 1;
        //shield break light
        case 160:
            return 1;
        //estus chug 1
        case 7585:
            return 1;
        //estus chug 2
        case 7586:
            return 1;
        //estus chug 3
        case 7587:
            return 1;
        //out of casts
        case 6299:
            return 1;
        //use item
        case 7500:
            return 1;
        //use humanity
        case 7501:
            return 1;
        //could not use
        case 7510:
            return 1;
        //throw
        case 7550:
            return 0;
        default:
            return 0;
    }
}

//HAHA! TIME FOR JANK!
//If the given animation is in this list, add up the two animation timers.
//NOTE:this requires the part 0 animation to be using timer 2, and the part 1 using timer 1
void CombineLastAnimation(int animation_id, AnimationCombineReturn* ret){
    switch (animation_id){
        //l hand combustion cast
        case 6207:
            ret->animationId = 6407;
            ret->partNumber = 0;
            break;
        case 6407:
            ret->animationId = 6407;
            ret->partNumber = 1;
            break;
        //r hand combustion cast
        case 6307:
            ret->animationId = 6507;
            ret->partNumber = 0;
            break;
        case 6507:
            ret->animationId = 6507;
            ret->partNumber = 1;
            break;
        //l hand wog cast
        case 6222:
            ret->animationId = 6422;
            ret->partNumber = 0;
            break;
        case 6422:
            ret->animationId = 6422;
            ret->partNumber = 1;
            break;
        //r hand wog cast
        case 6322:
            ret->animationId = 6522;
            ret->partNumber = 0;
            break;
        case 6522:
            ret->animationId = 6522;
            ret->partNumber = 1;
            break;
        default:
            ret->animationId = 0;
            ret->partNumber = 0;
            break;
    }
}


float dodgeTimings(int animation_id){
    switch (animation_id){
    case 100: return 0.5;//catch all for all kicks
    case 6407: return 0.65;//combined version of two combustion animations
    case 6507: return 0.65;//rhand of above
    case 6422: return 0.7;//combined version of two wog animations
    case 6522: return 0.7;//rhand of above
    case 6517: return 1.1;
    case 203000: return 0.25;
    case 203001: return 0.229667;
    case 203002: return 0.324;
    case 203300: return 0.427;
    case 203310: return 0.426;
    case 203500: return 0.529;
    case 203900: return 0.2;
    case 204000: return 0.3;
    case 204001: return 0.222;
    case 204002: return 0.324;
    case 204300: return 0.392;
    case 204310: return 0.409;
    case 204500: return 0.528;
    case 204600: return 0.8;
    case 204900: return 0.133;
    case 233000: return 0.51;
    case 233001: return 0.442;
    case 233002: return 0.442;
    case 233300: return 0.714;
    case 233301: return 0.720937;
    case 233310: return 0.34;
    case 233500: return 0.51;
    case 233900: return 2.494666;
    case 234000: return 0.529333;
    case 234001: return 0.476;
    case 234002: return 0.51;
    case 234300: return 0.578;
    case 234310: return 0.51;
    case 234500: return 0.544;
    case 234900: return 0.23;
    case 253000: return 0.648;
    case 253001: return 0.588;
    case 253002: return 0.652;
    case 253300: return 0.896;
    case 253310: return 0.796;
    case 253500: return 0.588;
    case 253600: return 1;
    case 253900: return 0.43;
    case 254000: return 0.588;
    case 254001: return 0.632;
    case 254002: return 0.592;
    case 254100: return 3.332664;
    case 254300: return 0.732;
    case 254310: return 0.896;
    case 254500: return 0.592;
    case 254600: return 1;
    case 254900: return 0.5;
    case 263000: return 0.956;
    case 263001: return 0.864;
    case 263002: return 0.96;
    case 263300: return 0.912;
    case 263301: return 0.91175;
    case 263310: return 0.912;
    case 263500: return 0.648;
    case 263900: return 2.999999;
    case 264000: return 0.744;
    case 264001: return 0.744;
    case 264002: return 0.744;
    case 264300: return 0.912;
    case 264301: return 0.9135;
    case 264310: return 0.912;
    case 264500: return 0.696;
    case 264900: return 0.55;
    case 273000: return 0.450292;
    case 273001: return 0.383688;
    case 273002: return 0.384479;
    case 273010: return 0.354604;
    case 273300: return 0.550417;
    case 273301: return 0.551042;
    case 273310: return 0.483938;
    case 273900: return 0.263646;
    case 274000: return 0.450688;
    case 274001: return 0.383771;
    case 274002: return 0.450625;
    case 274300: return 0.550542;
    case 274301: return 0.550583;
    case 274310: return 0.550583;
    case 274900: return 0.250375;
    case 283000: return 0.450458;
    case 283001: return 0.417208;
    case 283002: return 0.450229;
    case 283300: return 1.0185;
    case 283301: return 1.017417;
    case 283310: return 0.68375;
    case 283500: return 0.483813;
    case 283900: return 0.383917;
    case 284000: return 0.35075;
    case 284001: return 0.316938;
    case 284002: return 0.350521;
    case 284300: return 0.852125;
    case 284301: return 0.851313;
    case 284310: return 0.550521;
    case 284500: return 0.484021;
    case 284900: return 0.416896;
    case 293000: return 0.418792;
    case 293001: return 0.483833;
    case 293002: return 0.417313;
    case 293300: return 0.784125;
    case 293301: return 0.784625;
    case 293310: return 0.784375;
    case 293500: return 0.417063;
    case 293900: return 0.216833;
    case 294000: return 0.417438;
    case 294001: return 0.484417;
    case 294002: return 0.41725;
    case 294300: return 1.018042;
    case 294301: return 1.018271;
    case 294310: return 1.017812;
    case 294500: return 0.4;
    case 294900: return 0.250146;
    case 303000: return 0.617792;
    case 303001: return 0.617708;
    case 303002: return 0.617333;
    case 303040: return 0.701208;
    case 303041: return 0.701042;
    case 303042: return 0.700667;
    case 303300: return 0.817458;
    case 303301: return 0.817708;
    case 303310: return 0.850416;
    case 303500: return 0.617;
    case 303540: return 0.700333;
    case 303900: return 0.485209;
    case 304000: return 0.617917;
    case 304001: return 0.617708;
    case 304002: return 0.617708;
    case 304040: return 0.701417;
    case 304041: return 0.701125;
    case 304042: return 0.701333;
    case 304300: return 0.818542;
    case 304301: return 0.818542;
    case 304310: return 0.751167;
    case 304500: return 0.617458;
    case 304900: return 0.484084;
    case 323000: return 0.851458;
    case 323001: return 0.85175;
    case 323002: return 0.852666;
    case 323300: return 1.186041;
    case 323301: return 1.186416;
    case 323310: return 1.1855;
    case 323500: return 0.78425;
    case 323900: return 0.585625;
    case 324000: return 0.851542;
    case 324001: return 0.851;
    case 324002: return 0.851167;
    case 324300: return 1.18525;
    case 324301: return 1.185541;
    case 324310: return 1.186166;
    case 324500: return 0.78475;
    case 324900: return 0.584792;
    case 333000: return 0.618;
    case 333001: return 0.617917;
    case 333002: return 0.651208;
    case 333310: return 0.819917;
    case 333500: return 0.617417;
    case 333900: return 0.484625;
    case 334000: return 0.617458;
    case 334001: return 0.617667;
    case 334002: return 0.617833;
    case 334310: return 0.851708;
    case 334500: return 0.61825;
    case 334900: return 0.484792;
    case 353000: return 0.986291;
    case 353001: return 0.987875;
    case 353002: return 0.985792;
    case 353300: return 1.252708;
    case 353301: return 1.254833;
    case 353310: return 1.2535;
    case 353500: return 0.785875;
    case 353900: return 0.584083;
    case 354000: return 0.81875;
    case 354001: return 0.820375;
    case 354002: return 0.82;
    case 354300: return 1.254833;
    case 354301: return 1.252958;
    case 354310: return 1.254375;
    case 354500: return 0.787375;
    case 363000: return 0.46;
    case 363001: return 0.554833;
    case 363002: return 0.555583;
    case 363010: return 0.425125;
    case 363300: return 0.759166;
    case 363301: return 0.75675;
    case 363310: return 0.756;
    case 363500: return 0.487167;
    case 363900: return 0.38675;
    case 364000: return 0.521417;
    case 364001: return 0.521167;
    case 364002: return 0.520917;
    case 364300: return 0.755667;
    case 364301: return 0.756333;
    case 364310: return 0.75525;
    case 364500: return 0.487333;
    case 364900: return 0.386667;
    case 383000: return 0.553333;
    case 383001: return 0.723167;
    case 383002: return 0.654333;
    case 383041: return 0.773917;
    case 383300: return 0.8565;
    case 383301: return 0.855917;
    case 383310: return 0.890083;
    case 383500: return 0.05025;
    case 383900: return 0.389167;
    case 384000: return 0.521167;
    case 384001: return 0.687583;
    case 384002: return 0.687667;
    case 384300: return 1.223833;
    case 384301: return 1.226999;
    case 384310: return 1.225749;
    case 384500: return 0.05025;
    case 384900: return 0.386833;
    case 423000: return 0.587917;
    case 423001: return 0.385833;
    case 423002: return 0.319583;
    case 423300: return 0.621583;
    case 423301: return 0.620167;
    case 423310: return 0.688083;
    case 423500: return 0.621417;
    case 423900: return 0.217917;
    case 503000: return 0.720833;
    case 503001: return 0.721666;
    case 503002: return 0.720667;
    case 503300: return 0.754917;
    case 503301: return 0.7555;
    case 503310: return 0.55325;
    case 503500: return 0.622083;
    case 503600: return 0.9;
    case 503900: return 0.4865;
    case 504000: return 0.720833;
    case 504001: return 0.75525;
    case 504002: return 0.723083;
    case 504300: return 0.755;
    case 504301: return 0.755083;
    case 504310: return 0.587833;
    case 504600: return 1.0;
    case 504900: return 0.319083;
    case 513000: return 0.717354;
    case 513001: return 0.718188;
    case 513002: return 0.717896;
    case 513300: return 1.117979;
    case 513301: return 1.119812;
    case 513310: return 0.817563;
    case 513900: return 0.485;
    case 514000: return 0.717646;
    case 514001: return 0.750771;
    case 514002: return 0.717375;
    case 514300: return 1.017729;
    case 514301: return 1.018083;
    case 514310: return 0.8175;
    case 514900: return 0.483771;
    case 533300: return 0.578;
    case 533301: return 0.590938;
    case 533310: return 0.646;
    case 534300: return 0.578;
    case 534310: return 0.51;
    case 534900: return 2.312;
    case 543310: return 0.51;
    case 544300: return 0.68;
    case 544310: return 0.51;
    case 553300: return 0.646;
    case 553310: return 0.442;
    case 554300: return 0.646;
    case 554310: return 0.476;
    case 563000: return 0.450438;
    case 563001: return 0.417438;
    case 563002: return 0.450354;
    case 563300: return 0.750979;
    case 563301: return 0.751396;
    case 563310: return 0.752083;
    case 563500: return 0.484313;
    case 563900: return 0.384104;
    case 564000: return 0.351542;
    case 564001: return 0.317813;
    case 564002: return 0.351479;
    case 564300: return 0.750917;
    case 564301: return 0.752938;
    case 564310: return 0.785917;
    case 564500: return 0.484729;
    case 564900: return 0.417354;
    case 573300: return 0.68;
    case 573310: return 0.612;
    case 574300: return 0.68;
    case 574310: return 0.51;
    case 583300: return 0.577333;
    case 583310: return 0.646;
    case 584300: return 1.122;
    case 584315: return 1.123333;
    case 584900: return 2.32;
    case 593300: return 0.920083;
    case 593301: return 0.918125;
    case 593310: return 0.951291;
    case 594300: return 0.918291;
    case 594301: return 0.918541;
    case 594310: return 0.95125;
    case 603300: return 0.818416;
    case 603301: return 0.818125;
    case 603310: return 0.717792;
    case 604300: return 0.717792;
    case 604301: return 0.727375;
    case 604310: return 0.685;
    case 613300: return 1.12125;
    case 613301: return 1.118208;
    case 613310: return 1.118666;
    case 614300: return 0.985125;
    case 614301: return 0.992625;
    case 614310: return 0.984833;
    case 623300: return 1.252708;
    case 623301: return 1.262916;
    case 623310: return 1.253583;
    case 633300: return 0.892917;
    case 633301: return 0.89125;
    case 633310: return 0.6885;
    case 634300: return 0.757833;
    case 634301: return 0.757083;
    case 634310: return 0.856917;
    case 643300: return 1.832082;
    case 643301: return 1.831165;
    case 643310: return 1.832415;
    case 644300: return 1.831665;
    case 644301: return 1.830375;
    case 644310: return 1.832749;
    case 653300: return 0.790083;
    case 653301: return 0.790083;
    case 653310: return 0.789583;
    case 663300: return 1.05875;
    case 663301: return 1.05875;
    case 663310: return 1.025583;
    case 664300: return 0.788917;
    case 664301: return 0.790333;
    case 664310: return 0.790083;
    case 673300: return 1.0925;
    case 673301: return 1.09225;
    case 673310: return 1.091666;
    case 673500: return 1.026167;
    case 674300: return 1.0245;
    case 674301: return 1.025;
    case 674310: return 1.025583;
    case 674500: return 1.058416;
    case 683300: return 1.157833;
    case 683301: return 1.16025;
    case 683310: return 1.161583;
    case 684300: return 0.82225;
    case 684301: return 0.822333;
    case 684310: return 1.628832;
    case 703300: return 1.933082;
    case 703301: return 1.919999;
    case 703310: return 1.917999;
    case 704300: return 2.23;
    case 723300: return 0.529;
    case 723301: return 0.524437;
    case 723310: return 0.461;
    case 724300: return 0.584;
    case 724310: return 0.426;
    case 733300: return 0.442;
    case 733310: return 0.85;
    case 734300: return 0.544;
    case 734310: return 0.681333;
    case 743300: return 0.752;
    case 743310: return 0.792;
    case 744300: return 0.732;
    case 744310: return 0.896;
    case 744900: return 0.4;
    case 753300: return 0.764;
    case 753310: return 0.728;
    case 754300: return 0.716;
    case 754310: return 0.728667;
    case 773300: return 0.792;
    case 773310: return 0.588;
    case 783300: return 1.931999;
    case 783301: return 1.91475;
    case 784300: return 2.095999;
    case 793300: return 1.12;
    case 793305: return 1.12;
    case 794300: return 1.26;
    case 794305: return 1.264;
    case 803300: return 1.715999;
    case 804300: return 1.691999;
    case 863300: return 0.788167;
    case 863301: return 0.789333;
    case 863310: return 0.51975;
    case 863900: return 0.486917;
    case 873300: return 1.829082;
    case 873301: return 1.827832;
    case 873310: return 1.827748;
    case 893300: return 0.683979;
    case 893301: return 0.684542;
    case 893310: return 0.686083;
    case 894300: return 1.085;
    case 894301: return 1.085646;
    case 894310: return 1.084333;
    case 903300: return 1.351666;
    case 903301: return 1.351458;
    case 903310: return 1.354187;
    case 904300: return 1.452396;
    case 904301: return 1.451958;
    case 904310: return 1.451146;
    case 953300: return 1.056;
    case 953310: return 1.056;
    case 954300: return 1.056;
    case 954310: return 1.056;
    case 963300: return 1.896001;
    case 963301: return 1.897438;
    case 964301: return 1.851072;
    case 964310: return 1.85053;
    case 973300: return 1.651999;
    case 973301: return 1.653063;
    case 974300: return 1.651999;
    case 983300: return 0.917354;
    case 983301: return 0.917646;
    case 983310: return 0.917979;
    case 984300: return 0.917458;
    case 984301: return 0.919396;
    case 984310: return 0.91725;
    case 993300: return 0.550521;
    case 993301: return 0.550896;
    case 993310: return 0.517813;
    case 994300: return 0.550646;
    case 994301: return 0.5505;
    case 994310: return 0.517833;
    case 1003300: return 1.218375;
    case 1003301: return 1.20175;
    case 1003310: return 2.186249;
    case 1004300: return 1.218166;
    case 1004301: return 1.21825;
    case 1004310: return 1.985687;
    case 1013300: return 1.218083;
    case 1013301: return 1.219812;
    case 1013310: return 1.518166;
    case 1014300: return 1.151396;
    case 1014301: return 1.151188;
    case 1014310: return 1.618812;
    case 1023000: return 0.3835;
    case 1023001: return 0.283417;
    case 1023002: return 0.383583;
    case 1023300: return 0.617292;
    case 1023301: return 0.617167;
    case 1023310: return 0.617292;
    case 1023340: return 0.700625;
    case 1023341: return 0.7005;
    case 1023350: return 0.700625;
    case 1024000: return 0.383833;
    case 1024001: return 0.317208;
    case 1024002: return 0.383667;
    case 1024300: return 0.616917;
    case 1024301: return 0.618;
    case 1024310: return 0.616917;
    case 1024340: return 0.700417;
    case 1024341: return 0.701333;
    case 1024350: return 0.700333;
    case 1033300: return 0.817833;
    case 1033301: return 0.819167;
    case 1033310: return 0.818167;
    case 1034300: return 1.085208;
    case 1034301: return 1.085333;
    case 1034310: return 1.552999;
    case 1043300: return 1.05225;
    case 1043301: return 1.051291;
    case 1043310: return 1.051541;
    case 1044300: return 0.986125;
    case 1044301: return 0.979;
    case 1044310: return 0.985;
    case 1053300: return 1.286124;
    case 1053301: return 1.286541;
    case 1053310: return 0.717875;
    case 1054300: return 1.553624;
    case 1054301: return 1.552916;
    case 1054310: return 1.551999;
    case 1064300: return 1.392374;
    case 1064301: return 1.386666;
    case 1064310: return 1.386541;
    case 1073000: return 0.684542;
    case 1073001: return 0.650917;
    case 1073002: return 0.684667;
    case 1073300: return 1.620291;
    case 1073301: return 1.618957;
    case 1073310: return 1.619291;
    case 1074000: return 0.517334;
    case 1074001: return 0.617667;
    case 1074002: return 0.5175;
    case 1074300: return 1.752916;
    case 1074301: return 1.752291;
    case 1074310: return 1.753708;
    case 1083000: return 0.55425;
    case 1083001: return 0.55525;
    case 1083002: return 0.554417;
    case 1084000: return 0.553583;
    case 1084001: return 0.554333;
    case 1084002: return 0.554833;
    case 1093000: return 0.722333;
    case 1093001: return 0.78975;
    case 1093002: return 0.788583;
    case 1094000: return 0.790166;
    case 1094001: return 0.7895;
    case 1094002: return 0.78975;
    case 1103300: return 1.091916;
    case 1103301: return 1.091166;
    case 1103310: return 1.097083;
    case 1104300: return 1.092;
    case 1104301: return 1.090833;
    case 1104310: return 1.0925;
    case 1123300: return 1.395749;
    case 1123301: return 1.392749;
    case 1123310: return 1.392166;
    case 1124300: return 1.190583;
    case 1124301: return 1.18375;
    case 1124310: return 1.194;
    case 1133300: return 0.617;
    case 1133301: return 0.61725;
    case 1133310: return 0.583917;
    case 1134300: return 0.583792;
    case 1134301: return 0.583959;
    case 1134310: return 0.584458;
    case 1143300: return 0.817625;
    case 1143301: return 0.817167;
    case 1143310: return 0.617417;
    case 1144300: return 0.850833;
    case 1144301: return 0.850875;
    case 1144310: return 0.717375;
    case 1153300: return 1.017375;
    case 1153301: return 1.018041;
    case 1153310: return 0.917583;
    case 1154300: return 0.817583;
    case 1154301: return 0.817208;
    case 1154310: return 1.051;
    case 1163000: return 0.717688;
    case 1163001: return 0.718;
    case 1163002: return 0.717667;
    case 1163300: return 0.917688;
    case 1163301: return 0.9185;
    case 1163310: return 0.917729;
    case 1163900: return 0.484021;
    case 1164000: return 0.717604;
    case 1164001: return 0.751021;
    case 1164002: return 0.717271;
    case 1164300: return 0.851292;
    case 1164301: return 0.852417;
    case 1164310: return 0.850875;
    case 1164900: return 0.483667;
    case 1183000: return 0.618708;
    case 1183001: return 0.617333;
    case 1183002: return 0.617833;
    case 1183040: return 0.70225;
    case 1183041: return 0.70075;
    case 1183042: return 0.701375;
    case 1183300: return 0.81825;
    case 1183301: return 0.818208;
    case 1183310: return 0.851583;
    case 1183500: return 0.617417;
    case 1183540: return 0.700833;
    case 1183900: return 0.484167;
    case 1184000: return 0.617708;
    case 1184001: return 0.617708;
    case 1184002: return 0.617625;
    case 1184040: return 0.70125;
    case 1184041: return 0.701292;
    case 1184042: return 0.701042;
    case 1184300: return 0.817875;
    case 1184301: return 0.817917;
    case 1184310: return 0.751292;
    case 1184500: return 0.617542;
    case 1184900: return 0.484208;
    case 1193300: return 0.583896;
    case 1193301: return 0.584604;
    case 1193310: return 0.584229;
    case 1194300: return 0.750896;
    case 1194301: return 0.751146;
    case 1194310: return 0.750729;
    case 1203300: return 0.699;
    case 1243300: return 1.052083;
    case 1243301: return 1.051625;
    case 1243310: return 0.685208;
    case 1244300: return 1.254166;
    case 1244301: return 1.249875;
    case 1244310: return 1.25325;
    case 1253300: return 1.185333;
    case 1253301: return 1.186166;
    case 1253310: return 1.185125;
    case 1254300: return 1.085666;
    case 1254301: return 1.085041;
    case 1254310: return 1.0855;
    case 1283300: return 1.650728;
    case 1283301: return 1.650437;
    case 1283310: return 1.652853;
    case 1284301: return 1.684958;
    case 1284310: return 1.686708;
    case 1303000: return 0.306;
    case 1303001: return 0.238;
    case 1303002: return 0.306;
    case 1303300: return 0.578;
    case 1303310: return 0.578;
    case 1303500: return 0.51;
    case 1303900: return 1.598001;
    case 1304000: return 0.306;
    case 1304001: return 0.238;
    case 1304002: return 0.306;
    case 1304300: return 0.748;
    case 1304310: return 0.748;
    case 1304500: return 0.51;
    case 1304900: return 1.598001;
    case 1314300: return 1.627999;
    case 1323000: return 0.450979;
    case 1323001: return 0.41725;
    case 1323002: return 0.451229;
    case 1323300: return 1.18475;
    case 1323301: return 1.184729;
    case 1323310: return 1.452146;
    case 1323500: return 0.484;
    case 1323900: return 0.383583;
    case 1324000: return 0.350333;
    case 1324001: return 0.317208;
    case 1324002: return 0.350854;
    case 1324301: return 1.485104;
    case 1324310: return 1.651958;
    case 1324500: return 0.483792;
    case 1324900: return 0.416979;
    case 1333300: return 1.084958;
    case 1333301: return 1.084875;
    case 1333310: return 1.085333;
    case 1334300: return 1.152416;
    case 1334301: return 1.15225;
    case 1344300: return 0.788833;
    case 1344301: return 0.790667;
    case 1344310: return 0.79;
    case 1354300: return 1.791999;
    default:      
        guiPrint(LocationDetection",4:ALERT: %d animation id not found", animation_id);
        return 0.21;
    }
}