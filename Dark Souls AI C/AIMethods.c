#include "AIMethods.h"

#pragma warning( disable: 4244 )//ignore dataloss conversion from double to long

char EnemyStateProcessing(Character * Player, Character * Phantom){
    char returnVar = EnemyNeutral;

	//if they are outside of their attack range
    float distanceByLine = distance(Player, Phantom);
    guiPrint(LocationJoystick",1:Distance:%f", distanceByLine);
    
    unsigned char AtkID = isAttackAnimation(Phantom->animationType_id);

    if (distanceByLine <= Phantom->weaponRange){
		//attack id will tell up if an attack is coming up soon. if so, we need to prevent going into a subroutine(attack), and wait for attack to fully start b4 entering dodge subroutine

		if (
            //if in an animation where subanimation is not used for hurtbox
            (AtkID == 3 && Phantom->subanimation <= AttackSubanimationActiveDuringHurtbox) ||
            //or animation where it is
            ((AtkID == 2 || AtkID == 4) && (Phantom->subanimation == AttackSubanimationWindupClosing || Phantom->subanimation == AttackSubanimationActiveDuringHurtbox))
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
        //will overwrite strafe subroutine
        OverrideLowPriorityDefenseSubroutines();

        guiPrint(LocationDetection",0:backstab state %d", BackStabStateDetected);
        if (BackStabStateDetected == 2){
            returnVar = InBSPosition;
        }
        //override saftey notice here if = 4
        /*else if (AtkID != 4){TEMP DISABLE
            returnVar = BehindEnemy;
        }*/
    }

    if (returnVar == EnemyNeutral){
        guiPrint(LocationDetection",0:not about to be hit (enemy animation type id:%d) (enemy subanimation id:%d)", Phantom->animationType_id, Phantom->subanimation);
    }
    return returnVar;
}

/* ------------- DODGE Actions ------------- */

void StandardRoll(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport){
    long curTime = clock();

    guiPrint(LocationState",0:dodge roll time:%d", (curTime - startTimeDefense));

    //ensure we actually enter dodge roll in game so another subanimation cant override it
    //or we get poise broken out
    if (Player->subanimation == LockInSubanimation || Player->subanimation == PoiseBrokenSubanimation || curTime > startTimeDefense + 900){
        guiPrint(LocationState",0:end dodge roll");
        subroutine_states[DodgeTypeIndex] = 0;
        subroutine_states[DodgeStateIndex] = 0;
        AppendLastSubroutineSelf(StandardRollId);
        return;
    }

    //roll
    //AUUUGH FUCK IT JUST HAMMER THE BUTTON
    if (curTime < startTimeDefense + 50 || 
       (curTime > startTimeDefense + 100 && curTime < startTimeDefense + 150) || 
       (curTime > startTimeDefense + 200 && curTime < startTimeDefense + 250) || 
       (curTime > startTimeDefense + 300 && curTime < startTimeDefense + 350))
    {
        guiPrint(LocationState",1:circle");
        iReport->lButtons = circle;
        //handle this subroutines intitation after a counterstrafe abort (handles being locked on)
        //this will cause this roll to occur in lockon state, but subroutine will exit without lockon. Nothing major
        if (Player->locked_on){
            iReport->lButtons += r3;
        }
    }

    //turning
    if (curTime > startTimeDefense + 10 && curTime < startTimeDefense + 300){
        double rollOffset = 100.0;
        //if we're behind enemy, but we have to roll, roll towards their back for potential backstab
        if (BackstabDetection(Player, Phantom, distance(Player, Phantom)) == 1){
            rollOffset = 0;
        }

        double angle = angleFromCoordinates(Player->loc_x, Phantom->loc_x, Player->loc_y, Phantom->loc_y) - rollOffset;
        angle = angle < 0 ? angle + 360 : angle;//wrap around

        //angle joystick
        longTuple move = angleToJoystick(angle);
        //Stupid bug with dark souls. Can only roll when one of these is very close to middle. Select whatever one is furthest
        int diffX = abs(move.first - MIDDLE);
        int diffY = abs(move.second - MIDDLE);
        if (diffX > diffY){
            iReport->wAxisX = move.first;
        } else{
            iReport->wAxisY = move.second;
        }

        guiPrint(LocationState",1:offset angle %f angle roll %f", rollOffset, angle);
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
        AppendLastSubroutineSelf(BackstepId);
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
        guiPrint(LocationState",1:strafe");
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
        AppendLastSubroutineSelf(CounterStrafeId);
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
        AppendLastSubroutineSelf(L1AttackId);
    }
}

#define TimeForR3ToTrigger 50
#define TimeForCameraToRotateAfterLockon 180//how much time we give to allow the camera to rotate.
#define TimeDeltaForGameRegisterAction 120
#define TotalTimeInSectoReverseRoll ((TimeForR3ToTrigger + TimeForCameraToRotateAfterLockon + TimeDeltaForGameRegisterAction + 50) / (float)CLOCKS_PER_SEC)//convert above CLOCKS_PER_SEC ticks to seconds

//reverse roll through enemy attack and roll behind their back
static void ReverseRollBS(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport, char attackInfo){
    long curTime = clock();
    guiPrint(LocationState",0:Reverse Roll BS time:%d", (curTime - startTimeDefense));

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

    //move towards enemy's back.
    if ((curTime > startTimeDefense + TimeForR3ToTrigger + TimeForCameraToRotateAfterLockon + TimeDeltaForGameRegisterAction)
        //ensure we got behind the enemy, as we could have rolled into them
        && attackInfo == BehindEnemy)
    {
        float angle = angleFromCoordinates(Player->loc_x, Phantom->loc_x, Player->loc_y, Phantom->loc_y);
        /*if (Phantom->rotation < angle){
            angle += fmod((Phantom->rotation - angle),90);
        } else{
            angle += fmod((angle - Phantom->rotation), 90);
        }*/
        longTuple joystickAngles = angleToJoystick(angle);
        iReport->wAxisX = joystickAngles.first;
        iReport->wAxisY = joystickAngles.second;
        guiPrint(LocationState",1:moving to bs");
    }

    if (
        (curTime > startTimeDefense + 600) ||
        //early emergency abort in case enemy attack while we try to go for bs after roll or we dont get behind them after roll
        ((curTime > startTimeDefense + TimeForR3ToTrigger + TimeForCameraToRotateAfterLockon + TimeDeltaForGameRegisterAction + 80) && ((attackInfo == ImminentHit) || (attackInfo != BehindEnemy)))
        )
    {
        guiPrint(LocationState",0:end ReverseRollBS");
        subroutine_states[DodgeTypeIndex] = 0;
        subroutine_states[DodgeStateIndex] = 0;
        AppendLastSubroutineSelf(ReverseRollBSId);
    }
}

//this is more of a bandaid to the fact that the ai is ever getting hit
static void ToggleEscape(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport){
    long curTime = clock();
    guiPrint(LocationState",0:Toggle Escape:%d", (curTime - startTimeDefense));

    if (curTime < startTimeDefense + 30){
        iReport->bHats = dleft;
    }

    if (curTime > startTimeDefense + 60){
        guiPrint(LocationState",0:end Toggle Escape");
        subroutine_states[DodgeTypeIndex] = 0;
        subroutine_states[DodgeStateIndex] = 0;
        AppendLastSubroutineSelf(ToggleEscapeId);
    }
}

static void PerfectBlock(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport){
    guiPrint(LocationState",0:Perfect Block");
    long curTime = clock();

    if (curTime < startTimeDefense + 30){
        iReport->lButtons = l1;
    }

    if (curTime > startTimeDefense + 60){
        guiPrint(LocationState",0:end Perfect Block");
        subroutine_states[DodgeTypeIndex] = 0;
        subroutine_states[DodgeStateIndex] = 0;
        AppendLastSubroutineSelf(PerfectBlockId);
    }
}

static void ParrySubroutine(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport){
    guiPrint(LocationState",0:Parry");
    long curTime = clock();

    if (curTime < startTimeDefense + 30){
        iReport->lButtons = l2;
    }

    if (curTime > startTimeDefense + 60){
        guiPrint(LocationState",0:end Parry");
        subroutine_states[DodgeTypeIndex] = 0;
        subroutine_states[DodgeStateIndex] = 0;
        AppendLastSubroutineSelf(ParryId);
    }
}


//initiate the dodge command logic. This can be either toggle escaping, rolling, or parrying.
void dodge(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport, char attackInfo, unsigned char DefenseChoice){
    if (!inActiveSubroutine() && Player->subanimation >= LockInSubanimation){
		//indicate we are in dodge subroutine
        //special mappings to decide between neural net desicion and logic
        switch (attackInfo){
            case ImminentHit:
                //if we got hit already, and are in a state we can't dodge from, toggle escape the next hit
                if (Player->subanimation == PoiseBrokenSubanimation && (Phantom->dodgeTimeRemaining > 0.2 && Phantom->dodgeTimeRemaining < 0.3)){
                    subroutine_states[DodgeTypeIndex] = ToggleEscapeId;
                }
                //while staggered, dont enter any subroutines
                if (Player->subanimation != PoiseBrokenSubanimation){
                    //if the reverse roll is close enough to put us behind the enemy and we have enough windup time to reverse roll
                    if (distance(Player, Phantom) <= 3 && TotalTimeInSectoReverseRoll < Phantom->dodgeTimeRemaining){
                        subroutine_states[DodgeTypeIndex] = ReverseRollBSId;
                    }
                    //if we dont have enough time to roll, and we didnt just toggle, perfect block
                    else if (Phantom->dodgeTimeRemaining < 0.15 && Phantom->dodgeTimeRemaining > 0 && last_subroutine_states_self[0] != ToggleEscapeId){
                        subroutine_states[DodgeTypeIndex] = PerfectBlockId;
                    }
                    //otherwise, normal roll
                    else{
                        subroutine_states[DodgeTypeIndex] = StandardRollId;
                    }
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
            case ToggleEscapeId:
                ToggleEscape(Player, Phantom, iReport);
                break;
            case PerfectBlockId:
                PerfectBlock(Player, Phantom, iReport);
                break;
            //should never be reached
            default:
                guiPrint(LocationState",0:ERROR Unknown dodge action attackInfo=%d\nDodgeNeuralNetChoice=%d\nsubroutine_states[DodgeTypeIndex]=%d", attackInfo, DefenseChoice, subroutine_states[DodgeTypeIndex]);
                subroutine_states[DodgeStateIndex] = 0;
                break;
        }
    }
}

/* ------------- ATTACK Actions ------------- */

#define inputDelayForStart 10//if we exit move forward and go into attack, need this to prevent kick
#define inputDelayForKick 50

static void ghostHit(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport){
    long curTime = clock();
    guiPrint(LocationState",0:ghost hit time:%d", (curTime - startTimeAttack));

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
        AppendLastSubroutineSelf(GhostHitId);
	}
}

static void deadAngle(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport){
    long curTime = clock();
    guiPrint(LocationState",0:sub dead angle time:%d", (curTime - startTimeAttack));

    double angle = angleFromCoordinates(Player->loc_x, Phantom->loc_x, Player->loc_y, Phantom->loc_y);

    //handle entering with lockon
    if (Player->locked_on && curTime < startTimeAttack + inputDelayForKick){
        iReport->lButtons += r3;
    }

    //if we enter from a roll, move to enter neutral animation so we don't kick
    if (isDodgeAnimation(Player->animationType_id)){
        longTuple move = angleToJoystick(angle);
        iReport->wAxisX = move.first;
        iReport->wAxisY = move.second;
        startTimeAttack = curTime;//reset start time when we exit dodge, so we know how long to hold buttons for
    }
    //hold attack button for a bit
    else if (curTime < startTimeAttack + inputDelayForKick){
        guiPrint(LocationState",1:r1");
        iReport->lButtons += r1;
    }
    //point X degreees off angle from directly towards enemy
    else if (curTime > startTimeAttack + inputDelayForKick){
        guiPrint(LocationState",1:angle towards enemy: %f", angle);
        angle = -20.0 + angle;
        angle = angle > 360 ? angle - 360 : angle;
        longTuple move = angleToJoystick(angle);
        iReport->wAxisX = move.first;
        iReport->wAxisY = move.second;
    }

    if (curTime > startTimeAttack + 500){
        subroutine_states[AttackStateIndex] = 0;
        subroutine_states[AttackTypeIndex] = 0;
        guiPrint(LocationState",0:end sub dead angle");
        AppendLastSubroutineSelf(GhostHitId);
    }
}

static startTimeHasntBeenReset = true;
static void backStab(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport){
    guiPrint(LocationState",0:backstab");
    long curTime = clock();

    //backstabs cannot be triggerd from queued action
    //move character towards enemy back to switch to neutral animation as soon as in ready state
    double angle = angleFromCoordinates(Player->loc_x, Phantom->loc_x, Player->loc_y, Phantom->loc_y);
    longTuple move = angleToJoystick(angle);
    iReport->wAxisX = move.first;
    iReport->wAxisY = move.second;

    //once backstab is possible (neutral), press r1
    if (Player->subanimation == SubanimationNeutral){
        iReport->lButtons = r1;
        if (startTimeHasntBeenReset){
            startTimeAttack = curTime; //reset start time to allow exit timeout
            startTimeHasntBeenReset = false;
        }
    }

    //end subanimation after too long moving, or too long holding r1
    //exit if we either got the bs, or we incorrectly detected it
    if (curTime > startTimeAttack + 100){
        startTimeHasntBeenReset = true;
        subroutine_states[AttackStateIndex] = 0;
        subroutine_states[AttackTypeIndex] = 0;
        guiPrint(LocationState",0:end backstab");
        AppendLastSubroutineSelf(BackstabId);
    }
}

#define inputDelayForStopMove 90

static void MoveUp(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport){
    //if we are not close enough, move towards 
    long curTime = clock();
    guiPrint(LocationState",0:move up time:%d", (curTime - startTimeAttack));

    if (Player->locked_on && curTime < startTimeAttack + 40){
        iReport->lButtons = r3;
    }

    if (curTime < startTimeAttack + inputDelayForStopMove){
        longTuple move = angleToJoystick(angleFromCoordinates(Player->loc_x, Phantom->loc_x, Player->loc_y, Phantom->loc_y));
        iReport->wAxisX = move.first;
        iReport->wAxisY = move.second;
    }

    if (curTime > startTimeAttack + inputDelayForStopMove){
        subroutine_states[AttackStateIndex] = 0;
        subroutine_states[AttackTypeIndex] = 0;
        guiPrint(LocationState",0:end sub");
        AppendLastSubroutineSelf(MoveUpId);
    }
}

static void twoHand(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport){
    long curTime = clock();
    guiPrint(LocationState",0:two hand time:%d", (curTime - startTimeAttack));

    if (curTime < startTimeAttack + 40){
        iReport->lButtons = triangle;
    }

    if (curTime > startTimeAttack + 40){
        subroutine_states[AttackStateIndex] = 0;
        subroutine_states[AttackTypeIndex] = 0;
        guiPrint(LocationState",0:end two hand");
        AppendLastSubroutineSelf(TwoHandId);
    }
}

//lock on roll back to keep distance: prevent bs's, attacks 
static void SwitchWeapon(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport){
    guiPrint(LocationState",0:Switch Weapon");
    long curTime = clock();

    if (curTime < startTimeAttack + 30){
        iReport->lButtons = r3;
    }
    else if (curTime < startTimeAttack + 300){
        iReport->wAxisY = YBOTTOM;
        iReport->bHats = dleft;
    }

    if (curTime > startTimeAttack + 500){
        guiPrint(LocationState",0:end Switch Weapon");
        subroutine_states[AttackTypeIndex] = 0;
        subroutine_states[AttackStateIndex] = 0;
        AppendLastSubroutineSelf(SwitchWeaponId);
    }
}

static void Heal(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport){
    guiPrint(LocationState",0:Heal");
    long curTime = clock();

    if (curTime < startTimeAttack + 30){
        iReport->lButtons = square;
    }

    //1830 ms to use db
    if (curTime > startTimeAttack + 1830){
        guiPrint(LocationState",0:end Heal");
        subroutine_states[AttackTypeIndex] = 0;
        subroutine_states[AttackStateIndex] = 0;
        AppendLastSubroutineSelf(HealId);
    }
}

//initiate the attack command logic. This can be a standard(physical) attack or a backstab.
void attack(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport, char attackInfo, unsigned char AttackNeuralNetChoice){
    //procede with subroutine if we are not in one already
    //special case for asyncronous backstabs.
    if ((!inActiveSubroutine() || attackInfo == InBSPosition) && Player->subanimation >= SubanimationRecover){
        //special mappings to decide between neural net desicion and logic, determine if we want to enter attack subroutine
        switch (attackInfo){
            //we are in a position to bs
            case InBSPosition:
                subroutine_states[AttackTypeIndex] = BackstabId;
                break;
            //dont attack if enemy in windup
            case EnemyInWindup:
                //do allow move up though
                if (AttackNeuralNetChoice == MoveUpId){
                    subroutine_states[AttackTypeIndex] = MoveUpId;
                }
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
            //two hand
            case TwoHandId:
                twoHand(Player, Phantom, iReport);
                break;
            case SwitchWeaponId:
                SwitchWeapon(Player, Phantom, iReport);
                break;
            case HealId:
                Heal(Player, Phantom, iReport);
                break;
            default:
                guiPrint(LocationState",0:ERROR Unknown attack action attackInfo=%d\nAttackNeuralNetChoice=%d\nsubroutine_states[AttackTypeIndex]=%d", attackInfo, AttackNeuralNetChoice, subroutine_states[AttackTypeIndex]);
                break;
        }
    }
}