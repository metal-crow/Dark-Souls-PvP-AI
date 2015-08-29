#include "AIMethods.h"

#pragma warning( disable: 4244 )//ignore dataloss conversion from double to long

unsigned char aboutToBeHit(Character * Player, Character * Phantom){
    //dont have to check anything if already in dodge subroutine
    if (inActiveDodgeSubroutine()){
        return 2;
    }

	//if they are outside of their attack range, we dont have to do anymore checks
    if (distance(Player, Phantom) <= Phantom->weaponRange){
		unsigned char AtkID = isAttackAnimation(Phantom->animation_id);
		//attack id will tell up if an attack is coming up soon. if so, we need to prevent going into a subroutine(attack), and wait for attack to fully start b4 entering dodge subroutine

		if (
            //if in an animation where subanimation is not used for hurtbox
            (AtkID == 3 && Phantom->subanimation < AttackSubanimationActiveAfterHurtbox) ||
            //or animation where it is
            (AtkID == 2 && Phantom->subanimation == AttackSubanimationWindupClosing)
			//and their attack will hit me(their rotation is correct and their weapon hitbox width is greater than their rotation delta)
			//&& (Phantom->rotation)>((Player->rotation) - 3.1) && (Phantom->rotation)<((Player->rotation) + 3.1)
		){
            printf("about to be hit (anim id:%d) (suban id:%d)\n", Phantom->animation_id, Phantom->subanimation);
			return 2;
		}
		//windup, attack coming
        else if (AtkID == 1 || (AtkID == 2 && Phantom->subanimation == AttackSubanimationWindup)){
			//printf("dont attack\n");
			return 1;
		}
	}

    //printf("not about to be hit (dodge subr st:%d) (anim id:%d) (suban id:%d)\n", subroutine_states[DodgeStateIndex], Phantom->animation_id, Phantom->subanimation);
	return 0;
}

/* ------------- DODGE Actions ------------- */

#define inputDelayForStopDodge 40

void StandardRoll(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport){
    printf("rolling\n");
    //TODO angle should increase for closer distances
    //dodge at a 5 degree angle
    double angle = angleFromCoordinates(Player->loc_x, Phantom->loc_x, Player->loc_y, Phantom->loc_y);
    angle += 5;
    //angle joystick
    longTuple move = angleToJoystick(angle);
    iReport->wAxisX = move.first;
    iReport->wAxisY = move.second;

    //after the joystick input, press circle to roll but dont hold circle, otherwise we run
    long curTime = clock();
    if (curTime < startTime + inputDelayForStopDodge){
        printf("circle\n");
        iReport->lButtons = circle;
    }

    if (
        (curTime > startTime + inputDelayForStopDodge) &&
        //if we've compleated the dodge move and we're in animation end state we can end
        (Player->subanimation == AttackSubanimationRecover)// ||
        //or we end if not in dodge type animation id, because we could get hit out of dodge subroutine
        //!isDodgeAnimation(Player->animation_id))
       ){
        printf("end dodge roll\n");
        subroutine_states[DodgeTypeIndex] = 0;
        subroutine_states[DodgeStateIndex] = 0;
    }
    //printf("dodge roll\n");
}

#define inputDelayForStopCircle 40

void Backstep(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport){
    printf("Backstep\n");
    long curTime = clock();

    if (curTime < startTime + inputDelayForStopCircle){
        iReport->lButtons = circle;
    }

    if (
        (curTime > startTime + inputDelayForStopCircle)// &&
        //if we've compleated the dodge move and we're in animation end state we can end
        //(Player->subanimation == AttackSubanimationRecover)// ||
        //or we end if not in dodge type animation id, because we could get hit out of dodge subroutine
        //!isDodgeAnimation(Player->animation_id))
        ){
        printf("end backstep\n");
        subroutine_states[DodgeTypeIndex] = 0;
        subroutine_states[DodgeStateIndex] = 0;
    }
}

#define inputDelayForStopStrafe 800

void CounterStrafe(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport){
    printf("CounterStrafe\n");
    long curTime = clock();

    //Hvae to use keycodes because Vjoy doesnt work with r3 button
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.time = 0;
    ip.ki.wVk = 0; //We're doing scan codes instead
    ip.ki.dwExtraInfo = 0;
    ip.ki.dwFlags = KEYEVENTF_SCANCODE;
    ip.ki.wScan = 0x18;  //Set a unicode character to use (O)

    //have to lock on to strafe
    if (curTime < startTime + 30){
        //iReport->lButtons = 0x0;// r3;
        SendInput(1, &ip, sizeof(INPUT));
    }
    //need a delay for dark souls to respond
    else if (curTime < startTime + 60){
        ip.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
        SendInput(1, &ip, sizeof(INPUT));
    }

    else if (curTime < startTime + inputDelayForStopStrafe){
        //TODO make this strafe in the same direction as the enemy strafe
        iReport->wAxisX = XLEFT;
        iReport->wAxisY = YTOP;
    }

    //disable lockon
    else if (curTime < startTime + inputDelayForStopStrafe + 30){
        //iReport->lButtons = 0x0; //r3;
        SendInput(1, &ip, sizeof(INPUT));
    }
    else if (curTime < startTime + inputDelayForStopStrafe + 60){
        ip.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
        SendInput(1, &ip, sizeof(INPUT));
    }

    //break early if we didnt lock on
    if (curTime > startTime + inputDelayForStopStrafe + 60 || (!Player->locked_on && curTime > startTime + 60)){
        printf("end CounterStrafe\n");
        subroutine_states[DodgeTypeIndex] = 0;
        subroutine_states[DodgeStateIndex] = 0;
    }
}

static void MoveUp(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport);

void L1Attack(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport){
    printf("L1\n");
    long curTime = clock();

    if (curTime < startTime + 30){
        double angle = angleFromCoordinates(Player->loc_x, Phantom->loc_x, Player->loc_y, Phantom->loc_y);
        longTuple move = angleToJoystick(angle);
        iReport->wAxisX = move.first;
        iReport->wAxisY = move.second;
        iReport->lButtons = l1;
    }

    if (curTime > startTime + 30){
        printf("end L1\n");
        subroutine_states[DodgeTypeIndex] = 0;
        subroutine_states[DodgeStateIndex] = 0;
    }
}

//initiate the dodge command logic. This can be either toggle escaping, rolling, or parrying.
void dodge(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport, unsigned char DefenseChoice){
    //printf("dodge %d\n", DefenseChoice);
	//procede with subroutine if we are not in one already
	if (!inActiveSubroutine()){
		//indicate we are in dodge subroutine
        subroutine_states[DodgeTypeIndex] = DefenseChoice ? DefenseChoice : 1;//default to 1 on instinct
		subroutine_states[DodgeStateIndex] = 1;
		//set time for this subroutine
		startTime = clock();
	}

    //if we're in the dodge subroutine
    if (inActiveDodgeSubroutine()){
        switch (subroutine_states[DodgeTypeIndex]){
            //standard roll
            case 1:
                StandardRoll(Player, Phantom, iReport);
                break;
            //backstep
            case 2:
                Backstep(Player, Phantom, iReport);
                break;
            //counter strafe
            case 3:
                CounterStrafe(Player, Phantom, iReport);
                break;
            case 4:
                MoveUp(Player, Phantom, iReport);
                break;
            case 5:
                L1Attack(Player, Phantom, iReport);
                break;
            //should never be reached, since we default to 1 if instinct dodging
            default:
                break;
        }
    }
}

/* ------------- ATTACK Actions ------------- */

#define inputDelayForKick 50
#define inputDelayForRotateBack 70

//TODO can get locked in if samina runs out and this method is called repeatedly
static void ghostHit(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport){
    long curTime = clock();

    double angle = angleFromCoordinates(Player->loc_x, Phantom->loc_x, Player->loc_y, Phantom->loc_y);

    //hold attack button for a bit
    if (curTime < startTime + inputDelayForKick){
        iReport->lButtons = r1;
    }

    //start rotate back to enemy(TODO this is too late)
    if (Player->subanimation == AttackSubanimationWindupClosing){
        printf("towards\n");
        longTuple move = angleToJoystick(angle);
        iReport->wAxisX = move.first;
        iReport->wAxisY = move.second;
    }

	//cant angle joystick immediatly, at first couple frames this will register as kick
    //after timeout, point away from enemy till end of windup
    else if (curTime >= startTime + inputDelayForKick){
        printf("away\n");
        angle = fabs(angle - 180.0);//TODO this doesnt work for some angles
        longTuple move = angleToJoystick(angle);
        iReport->wAxisX = move.first;
        iReport->wAxisY = move.second;
	}

	//end subanimation on recover animation
    if (
        (curTime > startTime + inputDelayForRotateBack) &&
        //if we've compleated the attack move and we're in animation end state we can end
        (Player->subanimation == AttackSubanimationRecover)// ||
        //or we end if not in attack type animation id, because we could get hit out of attack subroutine
        //!isAttackAnimation(Player->animation_id))
    ){
        subroutine_states[AttackStateIndex] = 0;
        subroutine_states[AttackTypeIndex] = 0;
		printf("end sub\n");
	}
}

#define inputDelayForStopMove 90

static void MoveUp(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport){
    //if we are not close enough, move towards 
    printf("move up\n");
    long curTime = clock();
    if (curTime < startTime + inputDelayForStopMove){
        longTuple move = angleToJoystick(angleFromCoordinates(Player->loc_x, Phantom->loc_x, Player->loc_y, Phantom->loc_y));
        iReport->wAxisX = move.first;
        iReport->wAxisY = move.second;
    }

    if (curTime > startTime + inputDelayForStopMove){
        subroutine_states[AttackStateIndex] = 0;
        subroutine_states[AttackTypeIndex] = 0;
        subroutine_states[DodgeStateIndex] = 0;
        subroutine_states[DodgeTypeIndex] = 0;
        printf("end sub\n");
    }
}

//initiate the attack command logic. This can be a standard(physical) attack or a backstab.
void attack(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport, unsigned char AttackChoice){
    //printf("attack %d\n", AttackChoice);
	//TODO need timing analysis. Opponent can move outside range during windup

    //procede with subroutine if we are not in one already
    if (!inActiveSubroutine()){
        //indicate we are in attack subroutine
        subroutine_states[AttackTypeIndex] = AttackChoice;
        subroutine_states[AttackStateIndex] = 1;
        //set time for this subroutine
        startTime = clock();
    }

    //if we're in the attack subroutine
    if (inActiveAttackSubroutine()){
        //Differentiate different attack subroutines based on neural net decision
        switch (subroutine_states[AttackTypeIndex]){
        //to move towards the opponent
        case 1:
            MoveUp(Player, Phantom, iReport);
            break;
        //ghost hits for normal attacks
        case 2:
            ghostHit(Player, Phantom, iReport);
            break;
        //should never reach, only way to get into method is >0 AttackChoice
        default:
            break;
        }
    }
}