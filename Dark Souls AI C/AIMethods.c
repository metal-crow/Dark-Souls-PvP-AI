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
			//if enemy is in attack animation, 
			AtkID>1
            //checking here if the hurtbox is created based on attack ids where hurtbox is immediate and not used with subanimation, or subanimation state(AttackSubanimationActive is pre hurtbox)
            && (AtkID == 3 || Phantom->subanimation == AttackSubanimationActive)
			//and their attack will hit me(their rotation is correct and their weapon hitbox width is greater than their rotation delta)
			//&& (Phantom->rotation)>((Player->rotation) - 3.1) && (Phantom->rotation)<((Player->rotation) + 3.1)
			){
			printf("about to be hit ");
			return 2;
		}
		//windup, attack coming
		else if (AtkID){
			printf("dont attack\n");
			return 1;
		}
	}

    printf("not about to be hit (dodge subr st:%d) (anim id:%d) (suban id:%d)\n", subroutine_states[DodgeStateIndex], Phantom->animation_id, Phantom->subanimation);
	return 0;
}

/* ------------- DODGE Actions ------------- */

#define inputDelayForDodge 30
#define inputDelayForStopDodge 50

void StandardRoll(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport){
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
    if ((curTime >= startTime + inputDelayForDodge) && (curTime < startTime + inputDelayForStopDodge)){
        iReport->lButtons = circle;
    }

    //only check once we've gotten dodge in, to prevent premature exit. Can exit once dodge state in recovry or hit and subanimation reset
    //also check if in dodge type animation id, because we could get knocked into subroutine b4 dodge starts
    if ((curTime > startTime + inputDelayForStopDodge) && 
        (Player->subanimation == AttackSubanimationRecover || Player->subanimation == AttackSubanimationWindup || !isDodgeAnimation(Player->animation_id))){
        printf(" end sub ");
        subroutine_states[DodgeTypeIndex] = 0;
        subroutine_states[DodgeStateIndex] = 0;
    }
    printf("dodge roll\n");
}

//initiate the dodge command logic. This can be either toggle escaping, rolling, or parrying.
void dodge(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport, unsigned char DefenseChoice){
    printf("dodge %d\n", DefenseChoice);
	//procede with subroutine if we are not in one already
	if (!inActiveSubroutine()){
		//indicate we are in dodge subroutine
        subroutine_states[DodgeTypeIndex] = DefenseChoice ? DefenseChoice : 1;//default to 1 on instinct
		subroutine_states[DodgeStateIndex] = 1;
		//set time for this subroutine
		startTime = clock();
	}

    //if we're in the dodge subroutine
    if (subroutine_states[DodgeStateIndex]){
        switch (subroutine_states[DodgeTypeIndex]){
            //standard roll
            case 1:
                StandardRoll(Player, Phantom, iReport);
                break;
            //should never be reached, since we default to 1 if instinct dodging
            default:
                break;
        }
    }
}

/* ------------- ATTACK Actions ------------- */

#define inputDelayForKick 30
#define inputDelayForRotateBack 60

static void ghostHit(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport){
	//always hold attack button
	iReport->lButtons = r1;

	//cant angle joystick immediatly, at first couple frames this will register as kick
	//wait time, then trigger next stage of routine
	//printf(" %d ", (long)clock());
	if (clock() >= startTime + inputDelayForKick){
        subroutine_states[AttackStateIndex] = 2;
	}
	//start rotate back(Player->subanimation == 65792 marks point where we can NO longer turn back, CANT use that as flag, need to track internally)
	if (clock() >= startTime + inputDelayForRotateBack){
        subroutine_states[AttackStateIndex] = 3;
	}

	//point away from enemy till end of windup
    if (Player->subanimation == AttackSubanimationWindup && subroutine_states[AttackStateIndex] == 2){
		printf("away");
		double angle = angleFromCoordinates(Player->loc_x, Phantom->loc_x, Player->loc_y, Phantom->loc_y);
		angle = fabs(angle - 180.0);//TODO this doesnt work for some angles
		longTuple move = angleToJoystick(angle);
		iReport->wAxisX = move.first;
		iReport->wAxisY = move.second;
	}

	//point towards enemy during active part of animation
    else if (Player->subanimation == AttackSubanimationWindup && subroutine_states[AttackStateIndex] == 3){
		printf("towards");
		double angle = angleFromCoordinates(Player->loc_x, Phantom->loc_x, Player->loc_y, Phantom->loc_y);
		longTuple move = angleToJoystick(angle);
		iReport->wAxisX = move.first;
		iReport->wAxisY = move.second;
	}

	//end subanimation on recover animation
	else if (Player->subanimation == AttackSubanimationRecover){
        subroutine_states[AttackStateIndex] = 0;
        subroutine_states[AttackTypeIndex] = 0;
		//release attack button and joystick
		iReport->lButtons = 0x000000000;
		iReport->wAxisX = MIDDLE;
		iReport->wAxisY = MIDDLE;
		printf("end sub");
	}
	printf("\n");
}

static void MoveUp(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport){
    //if we are not close enough, move towards 
    longTuple move = angleToJoystick(angleFromCoordinates(Player->loc_x, Phantom->loc_x, Player->loc_y, Phantom->loc_y));
    iReport->wAxisX = move.first;
    iReport->wAxisY = move.second;
}

//initiate the attack command logic. This can be a standard(physical) attack or a backstab.
void attack(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport, unsigned char AttackChoice){
    printf("attack %d\n", AttackChoice);
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
    if (subroutine_states[AttackStateIndex]){
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