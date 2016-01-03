#include "AIMethods.h"

#pragma warning( disable: 4244 )//ignore dataloss conversion from double to long

char EnemyStateProcessing(Character * Player, Character * Phantom){
    char returnVar = EnemyNeutral;

	//if they are outside of their attack range
    float distanceByLine = distance(Player, Phantom);
    guiPrint(LocationJoystick",1:Distance:%f", distanceByLine);

    if (distanceByLine <= Phantom->weaponRange){
		unsigned char AtkID = isAttackAnimation(Phantom->animationType_id);
		//attack id will tell up if an attack is coming up soon. if so, we need to prevent going into a subroutine(attack), and wait for attack to fully start b4 entering dodge subroutine

		if (
            //if in an animation where subanimation is not used for hurtbox
            (AtkID == 3 && Phantom->subanimation <= AttackSubanimationActiveDuringHurtbox) ||
            //or animation where it is
            (AtkID == 2 && Phantom->subanimation == AttackSubanimationWindupClosing)
			//TODO and their attack will hit me(their rotation is correct and their weapon hitbox width is greater than their rotation delta)
			//&& (Phantom->rotation)>((Player->rotation) - 3.1) && (Phantom->rotation)<((Player->rotation) + 3.1)
		){
            OverrideLowPrioritySubroutines();
            guiPrint(LocationDetection",0:about to be hit (anim type id:%d) (suban id:%d)", Phantom->animationType_id, Phantom->subanimation);
            returnVar = ImminentHit;
		}
		//windup, attack coming
        //TODO should start to plan an attack now and attack while they;re attacking while avoiding the attack
        else if (AtkID == 1 || (AtkID == 2 && Phantom->subanimation == AttackSubanimationWindup)){
			guiPrint(LocationDetection",0:dont attack, enemy windup");
            returnVar = EnemyInWindup;
		}
	}

    //backstab checks. If AI CANNOT be attacked/BS'd, cancel Defense Neural net desicion. Also override attack neural net if can bs.
    unsigned char BackStabStateDetected = BackstabDetection(Player, Phantom, distanceByLine);
    if (BackStabStateDetected){
        guiPrint(LocationDetection",0:backstab detection result %d", BackStabStateDetected);
        //will overwrite strafe subroutine
        OverrideLowPrioritySubroutines();
        if (BackStabStateDetected == 2){
            returnVar = InBSPosition;
        } else{
            returnVar = BehindEnemy;
        }
    }

    if (returnVar == EnemyNeutral){
        guiPrint(LocationDetection",0:not about to be hit (enemy animation type id:%d) (enemy subanimation id:%d)", Phantom->animationType_id, Phantom->subanimation);
    }
    return returnVar;
}

/* ------------- DODGE Actions ------------- */

#define inputDelayForStopRotate 50
#define inputDelayForStopDodge 40

void StandardRoll(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport){
    long curTime = clock();

    guiPrint(LocationState",0:dodge roll");

    //this shouldnt have ever worked (100% anyway)
    double angle = angleFromCoordinates(Player->loc_x, Phantom->loc_x, Player->loc_y, Phantom->loc_y) - 40.0;//To avoid taking too long in turning, only turn 40 degrees max
    angle = angle < 0 ? angle + 360 : angle;
    //angle joystick
    longTuple move = angleToJoystick(angle);
    iReport->wAxisX = move.first;
    iReport->wAxisY = move.second;

    //IMPORTANT: CANNOT ROLL WHILE IN THE MIDDLE OF TURNING ROTATION. Have to wait until done turning before rolling
    //also, we're recalculating the direction to rotate to from our current direction, which was affected by the last rotation calculation. Minor issue.
    /*if (curTime < startTimeDefense + inputDelayForStopRotate){
        double angle = angleFromCoordinates(Player->loc_x, Phantom->loc_x, Player->loc_y, Phantom->loc_y) - 0.0;//To avoid taking too long in turning, only turn 90 degrees max
        angle = angle < 0 ? angle + 360 : angle;
        //angle joystick
        longTuple move = angleToJoystick(angle);
        iReport->wAxisX = move.first;
        iReport->wAxisY = move.second;
    }*/

    //after the joystick input, press circle to roll but dont hold circle, otherwise we run
    if (curTime < startTimeDefense + inputDelayForStopDodge){
        guiPrint(LocationState",1:circle");
        iReport->lButtons = circle;
        //handle this subroutines intitation after a counterstrafe abort (handles being locked on)
        //this will cause this roll to occur in lockon state, but subroutine will exit without lockon. Nothing major
        if (Player->locked_on){
            iReport->lButtons += r3;
        }
    }

    //need time to actually enter dodge roll in game so another subanimation cant override it
    if (curTime > startTimeDefense + inputDelayForStopDodge + 200){
        guiPrint(LocationState",0:end dodge roll");
        subroutine_states[DodgeTypeIndex] = 0;
        subroutine_states[DodgeStateIndex] = 0;
    }
}

#define inputDelayForStopCircle 40

void Backstep(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport){
    guiPrint(LocationState",0:Backstep");
    long curTime = clock();

    if (curTime < startTimeDefense + inputDelayForStopCircle){
        iReport->lButtons = circle;
    }

    if (
        (curTime > startTimeDefense + inputDelayForStopCircle)// &&
        //if we've compleated the dodge move and we're in animation end state we can end
        //(Player->subanimation == SubanimationRecover)// ||
        //or we end if not in dodge type animation id, because we could get hit out of dodge subroutine
        //!isDodgeAnimation(Player->animation_id))
        ){
        guiPrint(LocationState",0:end backstep");
        subroutine_states[DodgeTypeIndex] = 0;
        subroutine_states[DodgeStateIndex] = 0;
    }
}

#define inputDelayForStopStrafe 800

void CounterStrafe(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport){
    guiPrint(LocationState",0:CounterStrafe");
    long curTime = clock();

    //have to lock on to strafe
    if (curTime < startTimeDefense + 30){
        iReport->lButtons = r3;
        guiPrint(LocationState",1:lockon cs");
    }
    //need a delay for dark souls to respond
    else if (curTime < startTimeDefense + 60){
        iReport->lButtons = 0;
    }

    else if (curTime < startTimeDefense + inputDelayForStopStrafe){
        //TODO make this strafe in the same direction as the enemy strafe
        iReport->wAxisX = XLEFT;
        iReport->wAxisY = MIDDLE / 2;//3/4 pushed up
    }

    //disable lockon
    else if (curTime < startTimeDefense + inputDelayForStopStrafe + 30){
        iReport->lButtons = r3;
        guiPrint(LocationState",1:un lockon");
    }
    else if (curTime < startTimeDefense + inputDelayForStopStrafe + 60){
        iReport->lButtons = 0;
    }

    //break early if we didnt lock on
    if (curTime > startTimeDefense + inputDelayForStopStrafe + 60 || (!Player->locked_on && curTime > startTimeDefense + 60)){
        guiPrint(LocationState",0:end CounterStrafe");
        subroutine_states[DodgeTypeIndex] = 0;
        subroutine_states[DodgeStateIndex] = 0;
    }
}

void L1Attack(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport){
    guiPrint(LocationState",0:L1");
    long curTime = clock();

    if (curTime < startTimeDefense + 30){
        double angle = angleFromCoordinates(Player->loc_x, Phantom->loc_x, Player->loc_y, Phantom->loc_y);
        longTuple move = angleToJoystick(angle);
        iReport->wAxisX = move.first;
        iReport->wAxisY = move.second;
        iReport->lButtons = l1;
    }

    if (curTime > startTimeDefense + 30){
        guiPrint(LocationState",0:end L1");
        subroutine_states[DodgeTypeIndex] = 0;
        subroutine_states[DodgeStateIndex] = 0;
    }
}

#define TimeForR3ToTrigger 50
#define TimeForCameraToRotateAfterLockon 180//how much time we give to allow the camera to rotate.
#define TimeDeltaForGameRegisterAction 120
#define TotalTimeInSectoReverseRoll ((TimeForR3ToTrigger + TimeForCameraToRotateAfterLockon + TimeDeltaForGameRegisterAction + 100) / (float)CLOCKS_PER_SEC)//convert above CLOCKS_PER_SEC ticks to seconds

//reverse roll through enemy attack and roll behind their back
static void ReverseRollBS(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport, char attackInfo){
    guiPrint(LocationState",0:Reverse Roll BS");
    long curTime = clock();

    //have to lock on to reverse roll (also handle for being locked on already)
    if (curTime > startTimeDefense && curTime < startTimeDefense + TimeForR3ToTrigger && !Player->locked_on){
        iReport->lButtons = r3;
        guiPrint(LocationState",1:lockon rrbs");
    }

    //backwards then circle to roll and omnistep via delockon
    if (curTime > startTimeDefense + TimeForR3ToTrigger + TimeForCameraToRotateAfterLockon &&
        curTime < startTimeDefense + TimeForR3ToTrigger + TimeForCameraToRotateAfterLockon + TimeDeltaForGameRegisterAction){
        iReport->wAxisY = YBOTTOM;//have to do this because reverse roll is impossible on non normal camera angles
        iReport->lButtons = r3 + circle;
        guiPrint(LocationState",1:reverse roll");
    }

    //move towards enemy back. Since the reverse roll ends at a about a 45 degree angle ajacent to their back, move in this way
    //Have to move to the side to get behind the enemys back
    //instead of going directly towards enemy, go at that direction +-10 degrees to make the end angle perpendicular to enemy's rotation
    if ((curTime > startTimeDefense + TimeForR3ToTrigger + TimeForCameraToRotateAfterLockon + TimeDeltaForGameRegisterAction)
        //ensure we got behind the enemy, as we could have roll into them
        && attackInfo == BehindEnemy)
    {
        float angle = angleFromCoordinates(Player->loc_x, Phantom->loc_x, Player->loc_y, Phantom->loc_y);
        if (Phantom->rotation < angle){
            angle += fmod((Phantom->rotation - angle),90);
        } else{
            angle += fmod((angle - Phantom->rotation), 90);
        }
        longTuple joystickAngles = angleToJoystick(angle);
        iReport->wAxisX = joystickAngles.first;
        iReport->wAxisY = joystickAngles.second;
        guiPrint(LocationState",1:moving to bs");
    }

    if (
        (curTime > startTimeDefense + 3000) ||
        //early emergency abort in case enemy attack while we try to go for bs after roll or we dont get behind them after roll
        ((curTime > startTimeDefense + TimeForR3ToTrigger + TimeForCameraToRotateAfterLockon + TimeDeltaForGameRegisterAction) && ((attackInfo == ImminentHit) || (attackInfo != BehindEnemy)))
        )
    {
        guiPrint(LocationState",0:end ReverseRollBS");
        subroutine_states[DodgeTypeIndex] = 0;
        subroutine_states[DodgeStateIndex] = 0;
    }
}

//initiate the dodge command logic. This can be either toggle escaping, rolling, or parrying.
void dodge(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport, char attackInfo, unsigned char DefenseChoice){
    if (!inActiveSubroutine() && Player->subanimation >= SubanimationRecover){
		//indicate we are in dodge subroutine
        //special mappings to decide between neural net desicion and logic
        switch (attackInfo){
            case ImminentHit:
                //if the reverse roll is close enough to put us behind the enemy and we have enough windup time to reverse roll
                if (distance(Player, Phantom) <= 3 && TotalTimeInSectoReverseRoll < Phantom->dodgeTimeRemaining){
                    subroutine_states[DodgeTypeIndex] = ReverseRollBSId;
                }
                //otherwise, normal roll
                else{
                    subroutine_states[DodgeTypeIndex] = StandardRollId;
                }
                break;
            //only defines backstab detection handling
            default:
                subroutine_states[DodgeTypeIndex] = DefenseChoice;
                break;
        }
		subroutine_states[DodgeStateIndex] = 1;
		//set time for this subroutine
		startTimeDefense = clock();
	}

    if (inActiveDodgeSubroutine()){
        switch (subroutine_states[DodgeTypeIndex]){
            case StandardRollId:
                StandardRoll(Player, Phantom, iReport);
                break;
            case BackstepId:
                Backstep(Player, Phantom, iReport);
                break;
            case CounterStrafeId:
                CounterStrafe(Player, Phantom, iReport);
                break;
            case ReverseRollBSId:
                ReverseRollBS(Player, Phantom, iReport, attackInfo);
                break;
            //should never be reached, since defense choice must always be >0
            default:
                guiPrint(LocationState",0:ERROR Unknown dodge action attackInfo=%d\nDodgeNeuralNetChoice=%d\nsubroutine_states[DodgeTypeIndex]=%d", attackInfo, DefenseChoice, subroutine_states[DodgeTypeIndex]);
                break;
        }
    }
}

/* ------------- ATTACK Actions ------------- */

#define inputDelayForStart 10//if we exit move forward and go into attack, need this to prevent kick
#define inputDelayForKick 40

static void ghostHit(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport){
    guiPrint(LocationState",0:ghost hit");
    long curTime = clock();

    double angle = angleFromCoordinates(Player->loc_x, Phantom->loc_x, Player->loc_y, Phantom->loc_y);

    //handle entering with lockon
    if (Player->locked_on && curTime < startTimeAttack + inputDelayForKick){
        iReport->lButtons += r3;
    }

    //hold attack button for a bit
    if ((curTime < startTimeAttack + inputDelayForKick) && (curTime > startTimeAttack + inputDelayForStart)){
        guiPrint(LocationState",1:r1");
        iReport->lButtons += r1;
    }


    //start rotate back to enemy
    if (Player->subanimation == AttackSubanimationWindupGhostHit){
        guiPrint(LocationState",1:towards");
        longTuple move = angleToJoystick(angle);
        iReport->wAxisX = move.first;
        iReport->wAxisY = move.second;
    }

	//cant angle joystick immediatly, at first couple frames this will register as kick
    //after timeout, point away from enemy till end of windup
    else if (curTime > startTimeAttack + inputDelayForKick){
        guiPrint(LocationState",1:away");
        angle = fmod((180.0 + angle), 360.0);
        longTuple move = angleToJoystick(angle);
        iReport->wAxisX = move.first;
        iReport->wAxisY = move.second;
	}

	//end subanimation on recover animation
    if (
        (curTime > startTimeAttack + 800) &&
        (Player->subanimation >= AttackSubanimationWindupGhostHit)
    ){
        subroutine_states[AttackStateIndex] = 0;
        subroutine_states[AttackTypeIndex] = 0;
        guiPrint(LocationState",0:end sub ghost hit");
	}
}

static void deadAngle(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport){
    guiPrint(LocationState",0:sub dead angle");
    long curTime = clock();

    double angle = angleFromCoordinates(Player->loc_x, Phantom->loc_x, Player->loc_y, Phantom->loc_y);

    //handle entering with lockon
    if (Player->locked_on && curTime < startTimeAttack + inputDelayForKick){
        iReport->lButtons += r3;
    }

    //hold attack button for a bit
    if ((curTime < startTimeAttack + inputDelayForKick) && (curTime > startTimeAttack + inputDelayForStart)){
        guiPrint(LocationState",1:r1");
        iReport->lButtons += r1;
    }

    //point 50 degreees off angle from directly towards enemy
    if (curTime > startTimeAttack + inputDelayForKick){
        guiPrint(LocationState",1:angle");
        angle = fmod((50.0 + angle), 360.0);
        longTuple move = angleToJoystick(angle);
        iReport->wAxisX = move.first;
        iReport->wAxisY = move.second;
    }

    if (curTime > startTimeAttack + 200){
        subroutine_states[AttackStateIndex] = 0;
        subroutine_states[AttackTypeIndex] = 0;
        guiPrint(LocationState",0:end sub dead angle");
    }
}

static void backStab(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport){
    guiPrint(LocationState",0:backstab");
    long curTime = clock();

    //hold attack button for a bit
    if (curTime < startTimeAttack + 40){
        iReport->lButtons = r1;
    }

    //end subanimation immediatly
    if (curTime > startTimeAttack + 40){
        subroutine_states[AttackStateIndex] = 0;
        subroutine_states[AttackTypeIndex] = 0;
        guiPrint(LocationState",0:end backstab");
    }
}

#define inputDelayForStopMove 90

static void MoveUp(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport){
    //if we are not close enough, move towards 
    guiPrint(LocationState",0:move up");
    long curTime = clock();
    if (curTime < startTimeAttack + inputDelayForStopMove){
        longTuple move = angleToJoystick(angleFromCoordinates(Player->loc_x, Phantom->loc_x, Player->loc_y, Phantom->loc_y));
        iReport->wAxisX = move.first;
        iReport->wAxisY = move.second;
    }

    if (curTime > startTimeAttack + inputDelayForStopMove){
        subroutine_states[AttackStateIndex] = 0;
        subroutine_states[AttackTypeIndex] = 0;
        guiPrint(LocationState",0:end sub");
    }
}

//initiate the attack command logic. This can be a standard(physical) attack or a backstab.
void attack(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport, char attackInfo, unsigned char AttackNeuralNetChoice){
	//TODO need timing analysis. Opponent can move outside range during windup

    //procede with subroutine if we are not in one already
    //special case for asyncronous backstabs.
    if ((!inActiveSubroutine() || attackInfo == InBSPosition) && Player->subanimation >= SubanimationRecover){
        //special mappings to decide between neural net desicion and logic, determine if we want to enter attack subroutine
        switch (attackInfo){
            //we are in a position to bs
            case InBSPosition:
                subroutine_states[AttackTypeIndex] = BackstabId;
                break;
            default:
                subroutine_states[AttackTypeIndex] = AttackNeuralNetChoice;
                break;
        }
        if (subroutine_states[AttackTypeIndex]){
            subroutine_states[AttackStateIndex] = 1;
            //set time for this subroutine
            startTimeAttack = clock();
        }
    }

    //may not actually enter subroutine
    if (inActiveAttackSubroutine()){
        //Differentiate different attack subroutines based on neural net decision
        switch (subroutine_states[AttackTypeIndex]){
            //to move towards the opponent
            case MoveUpId:
                MoveUp(Player, Phantom, iReport);
                break;
            //ghost hits for normal attacks
            case GhostHitId:
                //ghostHit(Player, Phantom, iReport);
                deadAngle(Player, Phantom, iReport);
                break;
            //backstab
            case BackstabId:
                backStab(Player, Phantom, iReport);
                break;
            default:
                guiPrint(LocationState",0:ERROR Unknown attack action attackInfo=%d\nAttackNeuralNetChoice=%d\nsubroutine_states[AttackTypeIndex]=%d", attackInfo, AttackNeuralNetChoice, subroutine_states[AttackTypeIndex]);
                break;
        }
    }
}