#include "AIDecisions.h"

char EnemyStateProcessing(){
	char returnVar = EnemyNeutral;

	//if they are outside of their attack range
	float distanceByLine = distance(&Player, &Enemy);
	guiPrint(LocationJoystick",1:Distance:%f", distanceByLine);

	unsigned char AtkID = isAttackAnimation(Enemy.animationType_id);

	if (distanceByLine <= Enemy.weaponRange){
		//attack id will tell up if an attack is coming up soon. if so, we need to prevent going into a subroutine(attack), and wait for attack to fully start b4 entering dodge subroutine

		if (
			//if in an animation where subanimation is not used for hurtbox
			(AtkID == 3 && Enemy.subanimation <= AttackSubanimationActiveDuringHurtbox) ||
			//or animation where it is
			((AtkID == 2 || AtkID == 4) && (Enemy.subanimation >= AttackSubanimationWindupClosing && Enemy.subanimation <= AttackSubanimationActiveDuringHurtbox))
			//TODO and their attack will hit me(their rotation is correct and their weapon hitbox width is greater than their rotation delta)
			//&& (Phantom->rotation)>((Player->rotation) - 3.1) && (Phantom->rotation)<((Player->rotation) + 3.1)
			){
			OverrideLowPrioritySubroutines();
			guiPrint(LocationDetection",0:about to be hit (anim type id:%d) (suban id:%d)", Enemy.animationType_id, Enemy.subanimation);
			returnVar = ImminentHit;
		}
		//windup, attack coming
		//TODO should start to plan an attack now and attack while they;re attacking while avoiding the attack
		else if (AtkID == 1 || ((AtkID == 2 || AtkID == 4) && Enemy.subanimation == AttackSubanimationWindup)){
			guiPrint(LocationDetection",0:dont attack, enemy windup");
			returnVar = EnemyInWindup;
		}
	}

	//backstab checks. If AI CANNOT be attacked/BS'd, cancel Defense Neural net desicion. Also override attack neural net if can bs.
	unsigned char BackStabStateDetected = BackstabDetection(&Player, &Enemy, distanceByLine);
	if (BackStabStateDetected){
		//will overwrite strafe subroutine
		OverrideLowPriorityDefenseSubroutines();

		guiPrint(LocationDetection",0:backstab state %d", BackStabStateDetected);
		if (BackStabStateDetected == 2){
			returnVar = InBSPosition;
		}
		//override saftey notice here if = 4
		/*else if (AtkID != 4){TEMP DISABLE b/c some weapon attack go behind enemy
		returnVar = BehindEnemy;
		}*/
	}

	if (returnVar == EnemyNeutral){
		guiPrint(LocationDetection",0:not about to be hit (enemy animation type id:%d) (enemy subanimation id:%d)", Enemy.animationType_id, Enemy.subanimation);
	}
	return returnVar;
}
