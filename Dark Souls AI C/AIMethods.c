#include "AIMethods.h"

#pragma warning( disable: 4244 )//ignore dataloss conversion from double to long

/* ------------- DODGE Actions ------------- */

static void StandardRoll(JOYSTICK_POSITION * iReport){
    long curTime = clock();

    guiPrint(LocationState",0:dodge roll time:%d", (curTime - startTimeDefense));

    //ensure we actually enter dodge roll in game so another subanimation cant override it
    //or we get poise broken out
    if (Player.subanimation == LockInSubanimation || Player.subanimation == PoiseBrokenSubanimation || curTime > startTimeDefense + 900){
        guiPrint(LocationState",0:end dodge roll");
        subroutine_states[DodgeStateIndex] = SubroutineExiting;
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
        if (Player.locked_on){
            iReport->lButtons += r3;
        }
    }

    //turning
    if (curTime > startTimeDefense + 10 && curTime < startTimeDefense + 300){
        double rollOffset = 90.0;
        //if we're behind enemy, but we have to roll, roll towards their back for potential backstab
		if (BackstabDetection(&Player, &Enemy, distance(&Player, &Enemy)) == 1){
            rollOffset = 0;
        }
        //if we just rolled but have to roll again, ensure we roll away so we dont get caught in r1 spam
        else if (last_subroutine_states_self[0] == StandardRollId){
            rollOffset = 120.0;
        }
		//if we had to toggle escape, they're probably comboing. Roll away
		else if (last_subroutine_states_self[0] == ToggleEscapeId){
			rollOffset = 120.0;
		}

        double angle = angleFromCoordinates(Player.loc_x, Enemy.loc_x, Player.loc_y, Enemy.loc_y) - rollOffset;
        angle = angle < 0 ? angle + 360 : angle;//wrap around

        //angle joystick
		longTuple move;
		angleToJoystick(angle, &move);
        //Stupid bug with dark souls. Can only roll when one of these is very close to middle. Select whatever one is furthest
		int diffX = abs(move.x_axis - MIDDLE);
		int diffY = abs(move.y_axis - MIDDLE);
        if (diffX > diffY){
			iReport->wAxisX = move.x_axis;
        } else{
			iReport->wAxisY = move.y_axis;
        }

        guiPrint(LocationState",1:offset angle %f angle roll %f", rollOffset, angle);
    }
}

#define inputDelayForStopCircle 40

static void Backstep(JOYSTICK_POSITION * iReport){
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
        subroutine_states[DodgeStateIndex] = SubroutineExiting;
        AppendLastSubroutineSelf(BackstepId);
    }
}

#define inputDelayForOmnistepWait 40
#define inputDelayForStopOmnistepJoystickDirection 40

static void Omnistep_Backwards(JOYSTICK_POSITION * iReport){
	guiPrint(LocationState",0:Omnistep Backwards");
    long curTime = clock();

    if (curTime < startTimeDefense + inputDelayForStopCircle){
		iReport->lButtons = circle;
	}
	//TODO kind of not working
	else if (curTime > startTimeDefense + inputDelayForStopCircle + inputDelayForOmnistepWait && curTime < startTimeDefense + inputDelayForStopCircle + inputDelayForOmnistepWait + inputDelayForStopOmnistepJoystickDirection){
		double angle = angleFromCoordinates(Player.loc_x, Enemy.loc_x, Player.loc_y, Enemy.loc_y);
		//angle joystick
		longTuple move;
		angleToJoystick(angle, &move);
		iReport->wAxisX = move.x_axis;
		iReport->wAxisY = move.y_axis;
	}
	else{
		guiPrint(LocationState",0:end Omnistep Backwards");
		subroutine_states[DodgeStateIndex] = SubroutineExiting;
		AppendLastSubroutineSelf(OmnistepBackwardsId);
	}
}

#define inputDelayForStopStrafe 800

static void CounterStrafe(JOYSTICK_POSITION * iReport, bool left_strafe){
    long curTime = clock();
    guiPrint(LocationState",0:CounterStrafe:%d", (curTime - startTimeDefense));
    float distanceBt = distance(&Player, &Enemy);

    //have to lock on to strafe
    if (curTime < startTimeDefense + 30){
        iReport->lButtons = r3;
        guiPrint(LocationState",1:lockon cs");
    }
    //need a delay for dark souls to respond
    else if (curTime < startTimeDefense + 60){
        iReport->lButtons = 0;
    }

    //keep going if we're behind enemy or very close to them: might get a bs
    else if (curTime < startTimeDefense + inputDelayForStopStrafe || BackstabDetection(&Player, &Enemy, distanceBt) == 1 || distanceBt < 1.3){
		if (left_strafe){
			iReport->wAxisX = XLEFT;
		}else{
			iReport->wAxisX = XRIGHT;
		}
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

    else{
        guiPrint(LocationState",0:end CounterStrafe");
        subroutine_states[DodgeStateIndex] = SubroutineExiting;
		if (left_strafe){
			AppendLastSubroutineSelf(CounterStrafeLeftId);
		}else{
			AppendLastSubroutineSelf(CounterStrafeRightId);
		}
    }

    //break early if we didnt lock on
    if (!Player.locked_on && curTime > startTimeDefense + 60){
        guiPrint(LocationState",0:end CounterStrafe");
        subroutine_states[DodgeStateIndex] = SubroutineExiting;
		if (left_strafe){
			AppendLastSubroutineSelf(CounterStrafeLeftId);
		}else{
			AppendLastSubroutineSelf(CounterStrafeRightId);
		}
	}
}

static void L1Attack(JOYSTICK_POSITION * iReport){
    guiPrint(LocationState",0:L1");
    long curTime = clock();

    if (curTime < startTimeDefense + 30){
        double angle = angleFromCoordinates(Player.loc_x, Enemy.loc_x, Player.loc_y, Enemy.loc_y);
		longTuple move;
		angleToJoystick(angle, &move);
		iReport->wAxisX = move.x_axis;
		iReport->wAxisY = move.y_axis;
        iReport->lButtons = l1;
    }

    if (curTime > startTimeDefense + 30){
        guiPrint(LocationState",0:end L1");
        subroutine_states[DodgeStateIndex] = SubroutineExiting;
        AppendLastSubroutineSelf(L1AttackId);
    }
}

//reverse roll through enemy attack and roll behind their back
static void ReverseRollBS(JOYSTICK_POSITION * iReport){
    long curTime = clock();
    guiPrint(LocationState",0:Reverse Roll BS time:%d", (curTime - startTimeDefense));

    //have to lock on to reverse roll (also handle for being locked on already)
    if (curTime < startTimeDefense + TimeForR3ToTrigger && !Player.locked_on){
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

    if (curTime > startTimeDefense + TimeForR3ToTrigger + TimeForCameraToRotateAfterLockon + TimeDeltaForGameRegisterAction)
    {
        guiPrint(LocationState",0:end ReverseRollBS");
        subroutine_states[DodgeStateIndex] = SubroutineExiting;
        AppendLastSubroutineSelf(ReverseRollBSId);
    }
}

//this is more of a bandaid to the fact that the ai is ever getting hit
static void ToggleEscape(JOYSTICK_POSITION * iReport){
    long curTime = clock();
    guiPrint(LocationState",0:Toggle Escape:%d", (curTime - startTimeDefense));

    if (curTime < startTimeDefense + 30){
        iReport->bHats = dleft;
    }

    if (curTime > startTimeDefense + 60){
        guiPrint(LocationState",0:end Toggle Escape");
        subroutine_states[DodgeStateIndex] = SubroutineExiting;
        AppendLastSubroutineSelf(ToggleEscapeId);
    }
}

static void PerfectBlock(JOYSTICK_POSITION * iReport){
    guiPrint(LocationState",0:Perfect Block");
    long curTime = clock();

    if (curTime < startTimeDefense + 30){
        iReport->lButtons = l1;
    }

    if (curTime > startTimeDefense + 60){
        guiPrint(LocationState",0:end Perfect Block");
        subroutine_states[DodgeStateIndex] = SubroutineExiting;
        AppendLastSubroutineSelf(PerfectBlockId);
    }
}

static void ParrySubroutine(JOYSTICK_POSITION * iReport){
    guiPrint(LocationState",0:Parry");
    long curTime = clock();

    if (curTime < startTimeDefense + 30){
        iReport->lButtons = l2;
    }

    if (curTime > startTimeDefense + 60){
        guiPrint(LocationState",0:end Parry");
        subroutine_states[DodgeStateIndex] = SubroutineExiting;
        AppendLastSubroutineSelf(ParryId);
    }
}


void dodge(JOYSTICK_POSITION * iReport, InstinctDecision* instinct_decision, unsigned char DefenseNeuralNetChoice){
	//if we're not in active subroutine and we can enter one
    if (!inActiveSubroutine() && Player.subanimation >= LockInSubanimation)
	{
		//instinct overides AiMethods, have to do immediate dodge
		if (instinct_decision->priority_decision == EnterDodgeSubroutine){
			subroutine_states[DodgeTypeIndex] = instinct_decision->subroutine_id.defenseid;
		}
		//AiMethod defines less immediate dodges
		else{
			subroutine_states[DodgeTypeIndex] = DefenseNeuralNetChoice;
        }

		subroutine_states[DodgeStateIndex] = SubroutineActive;
		//set time for this subroutine
		startTimeDefense = clock();
	}

    if (inActiveDodgeSubroutine()){
        switch (subroutine_states[DodgeTypeIndex]){
            case StandardRollId:
                StandardRoll(iReport);
                break;
            case BackstepId:
                Backstep(iReport);
				break;
			case OmnistepBackwardsId:
				Omnistep_Backwards(iReport);
				break;
            case CounterStrafeLeftId:
                CounterStrafe(iReport, true);
                break;
			case CounterStrafeRightId:
				CounterStrafe(iReport, false);
				break;
			case L1AttackId:
				L1Attack(iReport);
				break;
            case ReverseRollBSId:
                ReverseRollBS(iReport);
                break;
            case ToggleEscapeId:
                ToggleEscape(iReport);
                break;
            case PerfectBlockId:
                PerfectBlock(iReport);
                break;
			case ParryId:
				ParrySubroutine(iReport);
				break;
            //may not do anything even though attack detected (ex we're staggered)
            default:
                subroutine_states[DodgeStateIndex] = NoSubroutineActive;
                break;
        }
    }
}

/* ------------- ATTACK Actions ------------- */

#define inputDelayForStart 10//if we exit move forward and go into attack, need this to prevent kick
#define inputDelayForKick 50

static void ghostHit(JOYSTICK_POSITION * iReport){
    long curTime = clock();
    guiPrint(LocationState",0:ghost hit time:%d", (curTime - startTimeAttack));

    double angle = angleFromCoordinates(Player.loc_x, Enemy.loc_x, Player.loc_y, Enemy.loc_y);

    //handle entering with lockon
    if (Player.locked_on && curTime < startTimeAttack + inputDelayForKick){
        iReport->lButtons += r3;
    }

    //hold attack button for a bit
    if ((curTime < startTimeAttack + inputDelayForKick) && (curTime > startTimeAttack + inputDelayForStart)){
        guiPrint(LocationState",1:r1");
        iReport->lButtons += r1;
    }

    //start rotate back to enemy
    if (Player.subanimation == AttackSubanimationWindupGhostHit){
        guiPrint(LocationState",1:towards");
		longTuple move;
		angleToJoystick(angle,&move);
		iReport->wAxisX = move.x_axis;
		iReport->wAxisY = move.y_axis;
    }

	//cant angle joystick immediatly, at first couple frames this will register as kick
    //after timeout, point away from enemy till end of windup
    else if (curTime > startTimeAttack + inputDelayForKick){
        guiPrint(LocationState",1:away");
        angle = fmod((180.0 + angle), 360.0);
		longTuple move;
		angleToJoystick(angle,&move);
		iReport->wAxisX = move.x_axis;
		iReport->wAxisY = move.y_axis;
	}

	//end subanimation on recover animation
    if (
        (curTime > startTimeAttack + 500) &&
        (Player.subanimation > AttackSubanimationWindupGhostHit)
    ){
        guiPrint(LocationState",0:end sub ghost hit");
        subroutine_states[AttackStateIndex] = SubroutineExiting;
        AppendLastSubroutineSelf(GhostHitId);
	}
}

static void deadAngle(JOYSTICK_POSITION * iReport){
    long curTime = clock();
    guiPrint(LocationState",0:sub dead angle time:%d", (curTime - startTimeAttack));

    double angle = angleFromCoordinates(Player.loc_x, Enemy.loc_x, Player.loc_y, Enemy.loc_y);

    //handle entering with lockon
    if (Player.locked_on && curTime < startTimeAttack + inputDelayForKick){
        iReport->lButtons += r3;
    }

    //if we enter from a roll, move to enter neutral animation so we don't kick
    if (isDodgeAnimation(Player.animationType_id)){
		longTuple move;
		angleToJoystick(angle,&move);
		iReport->wAxisX = move.x_axis;
		iReport->wAxisY = move.y_axis;
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
        angle = -60.0 + angle;
        angle = angle > 360 ? angle - 360 : angle;
		longTuple move;
		angleToJoystick(angle,&move);
		iReport->wAxisX = move.x_axis;
		iReport->wAxisY = move.y_axis;
    }

    if (curTime > startTimeAttack + 500){
        guiPrint(LocationState",0:end sub dead angle");
        subroutine_states[AttackStateIndex] = SubroutineExiting;
        AppendLastSubroutineSelf(GhostHitId);
    }
}

static startTimeHasntBeenReset = true;
static void backStab(JOYSTICK_POSITION * iReport){
    guiPrint(LocationState",0:backstab");
    long curTime = clock();

    //backstabs cannot be triggerd from queued action
    //move character towards enemy back to switch to neutral animation as soon as in ready state
    double angle = angleFromCoordinates(Player.loc_x, Enemy.loc_x, Player.loc_y, Enemy.loc_y);
	longTuple move;
	angleToJoystick(angle,&move);
	iReport->wAxisX = move.x_axis;
	iReport->wAxisY = move.y_axis;

    //once backstab is possible (neutral), press r1
    if (Player.subanimation == SubanimationNeutral){
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
        guiPrint(LocationState",0:end backstab");
        subroutine_states[AttackStateIndex] = SubroutineExiting;
        AppendLastSubroutineSelf(BackstabId);
    }
}

#define inputDelayForStopMove 90

static void MoveUp(JOYSTICK_POSITION * iReport){
    //if we are not close enough, move towards 
    long curTime = clock();
    guiPrint(LocationState",0:move up time:%d", (curTime - startTimeAttack));

    if (Player.locked_on && curTime < startTimeAttack + 40){
        iReport->lButtons = r3;
    }

    if (curTime < startTimeAttack + inputDelayForStopMove){
		longTuple move;
		angleToJoystick(angleFromCoordinates(Player.loc_x, Enemy.loc_x, Player.loc_y, Enemy.loc_y),&move);
		iReport->wAxisX = move.x_axis;
		iReport->wAxisY = move.y_axis;
    }

    if (curTime > startTimeAttack + inputDelayForStopMove){
        guiPrint(LocationState",0:end sub move up");
        subroutine_states[AttackStateIndex] = SubroutineExiting;
        AppendLastSubroutineSelf(MoveUpId);
    }
}

static void twoHand(JOYSTICK_POSITION * iReport){
    long curTime = clock();
    guiPrint(LocationState",0:two hand time:%d", (curTime - startTimeAttack));

    if (curTime < startTimeAttack + 40){
        iReport->lButtons = triangle;
    }

    if (curTime > startTimeAttack + 40){
        guiPrint(LocationState",0:end two hand");
        subroutine_states[AttackStateIndex] = SubroutineExiting;
        AppendLastSubroutineSelf(TwoHandId);
    }
}

//lock on roll back to keep distance: prevent bs's, attacks 
static void SwitchWeapon(JOYSTICK_POSITION * iReport){
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
        subroutine_states[AttackStateIndex] = SubroutineExiting;
        AppendLastSubroutineSelf(SwitchWeaponId);
    }
}

static void Heal(JOYSTICK_POSITION * iReport){
    guiPrint(LocationState",0:Heal");
    long curTime = clock();

    if (curTime < startTimeAttack + 30){
        iReport->lButtons = square;
    }

    //sometimes game doesnt register the heal. retry till it does.
    if (Player.subanimation != LockInSubanimation && curTime > startTimeAttack + 100){
        startTimeAttack = curTime;
    }

    //1830 ms to use db
    if (curTime > startTimeAttack + 1830){
        guiPrint(LocationState",0:end Heal");
        subroutine_states[AttackStateIndex] = SubroutineExiting;
        AppendLastSubroutineSelf(HealId);
    }
}

static double StartingPivotAngle = -1;
static long BehindStartTime = -1;
static void PivotBS(JOYSTICK_POSITION * iReport){
	long curTime = clock();
	guiPrint(LocationState",0:Pivot BS Time:%d", (curTime - startTimeAttack));

	float dist = distance(&Player, &Enemy);
	unsigned char bsState = BackstabDetection(&Player, &Enemy, dist);

	//de lock on if locked on
	if (Player.locked_on && curTime < startTimeAttack + 40){
		iReport->lButtons = r3;
	}

	//sprint up while in front of enemy
	if (bsState == 0){
		iReport->lButtons += circle;

		//save the starting angle so we dont constantly reangle
		if (StartingPivotAngle == -1){
			StartingPivotAngle = angleFromCoordinates(Player.loc_x, Enemy.loc_x, Player.loc_y, Enemy.loc_y) - 10;//run to their right
			StartingPivotAngle = StartingPivotAngle < 0 ? StartingPivotAngle + 360 : StartingPivotAngle;//wrap around
			guiPrint(LocationState",1:%f", StartingPivotAngle);
		}

		longTuple move;
		angleToJoystick(StartingPivotAngle,&move);
		iReport->wAxisX = move.x_axis;
		iReport->wAxisY = move.y_axis;
	}

	//when behind enemy (with enough space), reposition to face their back
	if (bsState == 1 && dist > 1){
		if (BehindStartTime == -1){
			BehindStartTime = curTime;
		}
		longTuple move;
		//to prevent skid from sudden angle change when we get behind enemy
		//decrement angle we're pointing at over time(degrees per ms) to smooth out transition
		double smoothingAngle = StartingPivotAngle + (curTime - BehindStartTime);
		smoothingAngle = smoothingAngle < 0 ? smoothingAngle + 360 : smoothingAngle;
		angleToJoystick(smoothingAngle, &move);
		iReport->wAxisX = move.x_axis;
		iReport->wAxisY = move.y_axis;
	}

	//end when we got a backstab or backstab avoidance is triggered(set threshold) or ???
	//reset BehindStartTime and StartingPivotAngle
}

void attack(JOYSTICK_POSITION * iReport, InstinctDecision* instinct_decision, unsigned char AttackNeuralNetChoice){
	//procede with subroutine if we are not in one already
	//special case for asyncronous backstabs.
	if ((!inActiveSubroutine() || instinct_decision->subroutine_id.attackid == BackstabId) && Player.subanimation >= SubanimationRecover)
	{
		if (instinct_decision->priority_decision == EnterAttackSubroutine){
			subroutine_states[AttackTypeIndex] = instinct_decision->subroutine_id.attackid;
		}
		//dont attack if enemy in windup
		else if (instinct_decision->priority_decision == DelayActions){
			//TODO should allow some longer term actions as long as they arn't attack here
			//do allow move up though
			if (AttackNeuralNetChoice == MoveUpId){
				subroutine_states[AttackTypeIndex] = MoveUpId;
			}
		}
		else{
			subroutine_states[AttackTypeIndex] = AttackNeuralNetChoice;
		}

        if (subroutine_states[AttackTypeIndex]){
            subroutine_states[AttackStateIndex] = 1;
            //set time for this subroutine
            startTimeAttack = clock();
        }
    }

    //may not actually enter subroutine
    if (inActiveAttackSubroutine()){
        switch (subroutine_states[AttackTypeIndex]){
            case MoveUpId:
                MoveUp(iReport);
                break;
            case GhostHitId:
                ghostHit(iReport);
                break;
            case DeadAngleId:
                deadAngle(iReport);
                break;
            case BackstabId:
                backStab(iReport);
                break;
            case TwoHandId:
                twoHand(iReport);
                break;
            case SwitchWeaponId:
                SwitchWeapon(iReport);
                break;
            case HealId:
                Heal(iReport);
                break;
			case PivotBSId:
				PivotBS(iReport);
				break;
            default:
                guiPrint(LocationState",0:ERROR Unknown attack action"
										"priority_decision=%d\n"
										"AttackNeuralNetChoice=%d\n"
										"subroutine_states[AttackTypeIndex]=%d", 
										instinct_decision->priority_decision, AttackNeuralNetChoice, subroutine_states[AttackTypeIndex]);
                break;
        }
    }
}