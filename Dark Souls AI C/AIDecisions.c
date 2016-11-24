#include "AIDecisions.h"

PriorityDecision PriorityDecisionMaking(){
	PriorityDecision returnVar = EnemyNeutral;

	//if they are outside of their attack range
	float distanceByLine = distance(&Player, &Enemy);
	guiPrint(LocationJoystick",1:Distance:%f", distanceByLine);

	unsigned char AtkID = isAttackAnimation(Enemy.animationType_id);

	//if enemy in range and we're not in invulnerable position (bs knockdown)
	if (distanceByLine <= Enemy.weaponRange && Player.animationType_id != KnockdownId){
		if (
			//if in an animation where subanimation is not used for hurtbox
			(AtkID == 3 && Enemy.subanimation <= AttackSubanimationActiveDuringHurtbox) ||
			//or animation where it is
			((AtkID == 2 || AtkID == 4) && (Enemy.subanimation >= AttackSubanimationWindupClosing && Enemy.subanimation <= AttackSubanimationActiveDuringHurtbox))
			//TODO and their attack will hit me(their rotation is correct and their weapon hitbox width is greater than their rotation delta)
			//&& (Phantom->rotation)>((Player->rotation) - 3.1) && (Phantom->rotation)<((Player->rotation) + 3.1)
			//TODO if enemy not in bs knockdown
			)
		{
			OverrideLowPrioritySubroutines();
			guiPrint(LocationDetection",0:about to be hit (anim type id:%d) (suban id:%d)", Enemy.animationType_id, Enemy.subanimation);
			returnVar = EnterDodgeSubroutine;
		}
		//windup, attack coming
		else if (AtkID == 1 || ((AtkID == 2 || AtkID == 4) && Enemy.subanimation == AttackSubanimationWindup)){
			guiPrint(LocationDetection",0:dont attack, enemy windup");
			returnVar = DelayActions;
		}
	}

	//backstab checks. If AI can BS, always take it
	unsigned char BackStabStateDetected = BackstabDetection(&Player, &Enemy, distanceByLine);
	if (BackStabStateDetected){
		OverrideLowPrioritySubroutines();

		guiPrint(LocationDetection",0:backstab state %d", BackStabStateDetected);
		if (BackStabStateDetected){
			returnVar = EnterAttackSubroutine;
		}
	}

	if (returnVar == EnemyNeutral){
		guiPrint(LocationDetection",0:not about to be hit (enemy animation type id:%d) (enemy subanimation id:%d)", Enemy.animationType_id, Enemy.subanimation);
	}
	return returnVar;
}
