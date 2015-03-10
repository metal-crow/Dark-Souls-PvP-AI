#include "AIMethods.h"

#pragma warning( disable: 4244 )//ignore dataloss conversion from double to long

//get straight line distance between me and enemy
static double distance(Character * Player, Character * Phantom){
	double delta_x = abs(Player->loc_x - Phantom->loc_x);
	double delta_y = abs(Player->loc_y - Phantom->loc_y);
	return hypot(delta_x, delta_y);
}

//determine what type of action the animation id is.
//i cant seem to find a pattern in the ids, so this is just a big switch statement
//ranged attacks use a different format: they have a specific animation for windup,recover, and hurtbox creation; while others rely on a subanimation id to determine windup, hurtbox, and recovery.
//because of this, have to specify if we need to look at subanimation
//0 is not attack animation, 1 is hurtbox is created, 2 is attack id but must check subanimation
static unsigned char isAttackAnimation(unsigned char animation_id){
	switch (animation_id){
	//1 hand roll
	case 32:
		return 0;
	//backstep 1h
	case 38:
		return 0;
	//1h rolling attack
	case 41:
		return 2;
	//1h r1
	case 46:
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
		return 1;
	//crossbow recover 2h
	case 68:
		return 0;
	//crossbow windup 1h
	case 69:
		return 0;
	//crossbow attack 1h
	case 70:
		return 1;
	//crossbow recover 1h
	case 71:
		return 0;
	//shield raise
	case 76:
		return 0;
	//shield up
	case 77:
		return 0;
	//shied lower
	case 78:
		return 0;
	//parry
	case 86:
		return 0;
	//1h r2
	case 89:
		return 2;
	//rolling attack 2h
	case 103:
		return 2;
	//2h r1
	case 107:
		return 2;
	//jumping 2 hand
	case 113:
		return 2;
	//2h r2
	case 115:
		return 2;
	//firestorm windup
	case 163:
		return 0;
	//firstorm
	case 164:
		return 2;
	//flame windup
	case 167:
		return 0;
	//flame attack
	case 168:
		return 2;
	//pyro ball windup
	case 173:
		return 0;
	//pyro ball throw
	case 174:
		return 2;
	//backstep 2h
	case 100:
		return 0;
	//1 hand weapon
	case 185:
		return 0;
	//1 hand weapon
	case 188:
		return 0;
	//2 hand weapon
	case 199:
		return 0;
	//2 hand weapon
	case 203:
		return 0;
	//weapon switch r
	case 209:
		return 0;
	//weapon switch r
	case 213:
		return 0;
	//weapon switch l
	case 217:
		return 0;
	//weapon switch l
	case 221:
		return 0;
	default:
		printf("unknown animation id\n");
		return 0;
	}
}

bool aboutToBeHit(Character * Player, Character * Phantom){
	//if they are outside of their attack range
	if (distance(Player, Phantom) > Phantom->weaponRange){
		return false;
	}
	unsigned char AtkID = isAttackAnimation(Phantom->animation_id);
	if (
		//if enemy is in attack animation, 
		AtkID
		//this is the range attack edge case or attack animation about to generate hurtbox(check sub animation)
		&& (AtkID == 1 || (Phantom->subanimation) == 256)
		//and their attack will hit me(their rotation is correct and their weapon hitbox width is greater than their rotation delta)
		&& (Phantom->rotation)>0.95 && (Phantom->rotation)<???
	){
		return true;
	}
}

//initiate the dodge command logic. This can be either toggle escaping, rolling, or parrying.
void dodge(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport){

}

//initiate the attack command logic. This can be a standard(physical) attack or a backstab.
void attack(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport){
	//if im farther away then my weapon can reach
	if (distance(Player, Phantom) > Player->weaponRange){
		//if we are not close enough, move towards 
		longTuple move = CoordsToJoystickAngle(Player->loc_x, Phantom->loc_x, Player->loc_y, Phantom->loc_y);
		iReport->wAxisX = move.first;
		iReport->wAxisY = move.second;
	}
	//otherwise, decide between attack and backstab

	//(always use ghost hits for normal attacks)
}