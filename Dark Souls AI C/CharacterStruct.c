#include "CharacterStruct.h"
#define PI 3.14159265

#pragma warning( disable: 4244 )//ignore dataloss conversion from double to float
#pragma warning( disable: 4305 )

void ReadPlayer(Character * c, HANDLE * processHandle, int characterId){
	HANDLE processHandle_nonPoint = *processHandle;
    //TODO read large block that contains all data, then parse in process
	//read x location
	ReadProcessMemory(processHandle_nonPoint, (LPCVOID)(c->location_x_address), &(c->loc_x), 4, 0);
    guiPrint("%d,0:X:%f", characterId, c->loc_x);
	//read y location
	ReadProcessMemory(processHandle_nonPoint, (LPCVOID)(c->location_y_address), &(c->loc_y), 4, 0);
    guiPrint("%d,1:Y:%f", characterId, c->loc_y);
	//read rotation of player
	ReadProcessMemory(processHandle_nonPoint, (LPCVOID)(c->rotation_address), &(c->rotation), 4, 0);
	//Player rotation is pi. 0 to pi,-pi to 0. Same as atan2
	//convert to radians, then to degrees
	c->rotation = (c->rotation + PI) * (180.0 / PI);
    guiPrint("%d,2:Rotation:%f", characterId, c->rotation);
	//read current animation type
    ReadProcessMemory(processHandle_nonPoint, (LPCVOID)(c->animationType_address), &(c->animationType_id), 2, 0);
    guiPrint("%d,3:Animation Type:%d", characterId, c->animationType_id);
	//read hp
	ReadProcessMemory(processHandle_nonPoint, (LPCVOID)(c->hp_address), &(c->hp), 4, 0);
    //read stamina
    if (c->stamina_address){
        ReadProcessMemory(processHandle_nonPoint, (LPCVOID)(c->stamina_address), &(c->stamina), 4, 0);
        guiPrint("%d,4:Stamina:%d", characterId, c->stamina);
    }
	//read what weapon they currently have in right hand
	ReadProcessMemory(processHandle_nonPoint, (LPCVOID)(c->r_weapon_address), &(c->r_weapon_id), 4, 0);
    guiPrint("%d,5:R Weapon:%d", characterId, c->r_weapon_id);
	//read what weapon they currently have in left hand
	ReadProcessMemory(processHandle_nonPoint, (LPCVOID)(c->l_weapon_address), &(c->l_weapon_id), 4, 0);
    guiPrint("%d,6:L Weapon:%d", characterId, c->l_weapon_id);

	//read if hurtbox is active on enemy weapon
    if (c->hurtboxActive_address){
        unsigned char hurtboxActiveState;
        ReadProcessMemory(processHandle_nonPoint, (LPCVOID)(c->hurtboxActive_address), &hurtboxActiveState, 1, 0);
        if (hurtboxActiveState){
            c->subanimation = AttackSubanimationActiveDuringHurtbox;
        }
    }
    int animationid;
    ReadProcessMemory(processHandle_nonPoint, (LPCVOID)(c->animationId_address), &animationid, 4, 0);
    guiPrint("%d,7:Animation Id:%d", characterId, animationid);

    unsigned char attackAnimationInfo = isAttackAnimation(c->animationType_id);

    if (isVulnerableAnimation(animationid))
    {
        c->subanimation = AttackSubanimationActiveHurtboxOver;
    }
    else if (isDodgeAnimation(c->animationType_id)){
        c->subanimation = DodgeSubanimation;
    }

    //read how long the animation has been active, check with current animation, see if hurtbox is about to activate
    //what i want is a countdown till hurtbox is active
    //cant be much higher b/c need spell attack timings
    //also check that this is an attack that involves subanimation
    else if (animationid > 1000 && attackAnimationInfo == 2){
        //if kick or parry, immediate dodge away (aid ends in 100)
        if (animationid % 1000 == 100){
            c->subanimation = AttackSubanimationWindupClosing;
        }
        else{
            float animationTimer;
            ReadProcessMemory(processHandle_nonPoint, (LPCVOID)(c->animationTimer_address), &animationTimer, 4, 0);

            float dodgeTimer = dodgeTimings(animationid);
            c->dodgeTime = dodgeTimer;
            float timeDelta = dodgeTimer - animationTimer;

            guiPrint("%d,8:Animation Timer:%f\nDodge Time:%f", characterId, animationTimer, dodgeTimer);

            if (timeDelta > 0.45){
                c->subanimation = AttackSubanimationWindup;
            }
            // time before the windup ends where we can still alter rotation
            else if (timeDelta < 0.14){
                c->subanimation = AttackSubanimationWindupGhostHit;
            }
            //between 0.45 and 0.15 sec b4 hurtbox. If we have less that 0.15 we can't dodge.
            else if (timeDelta <= 0.45 && timeDelta >= 0.15){
                c->subanimation = AttackSubanimationWindupClosing;
            }
        }
    }
    else if (attackAnimationInfo == 1){
        c->subanimation = AttackSubanimationWindup;
    }
    else if (attackAnimationInfo == 3){
        c->subanimation = AttackSubanimationActiveDuringHurtbox;
    }
    else{
    //else if (c->animationType_id == 0){//0 when running, walking, standing. all animation can immediatly transition to new animation
        c->subanimation = SubanimationNeutral;
    }

    //read if in ready state(can transition to another animation)
    //Can sometimes be 0 even when animation in is changable. use in conjunction with animation type id
    if (c->readyState_address){
        unsigned char readyState;
        ReadProcessMemory(processHandle_nonPoint, (LPCVOID)(c->readyState_address), &readyState, 1, 0);
        if(readyState){
            c->subanimation = SubanimationRecover;
        }
    }
    guiPrint("%d,9:Subanimation:%d", characterId, c->subanimation);

    //read the current velocity
    //player doesnt use this, and wont have the address set. enemy will
    if (c->velocity_address){
        ReadProcessMemory(processHandle_nonPoint, (LPCVOID)(c->velocity_address), &(c->velocity), 4, 0);
        guiPrint("%d,10:Velocity:%f", characterId, c->velocity);
    }
    //read if the player is locked on
    if (c->locked_on_address){
        ReadProcessMemory(processHandle_nonPoint, (LPCVOID)(c->locked_on_address), &(c->locked_on), 1, 0);
        guiPrint("%d,11:Locked On:%d", characterId, c->locked_on);
    }
}

void ReadPlayerDEBUGGING(Character * c, HANDLE * processHandle, ...){
    c->loc_x = 1045.967773;
    c->loc_y = 864.3547974;
    c->rotation = 360;//facing kinda towards bonfire, same as pi/-pi
    c->animationType_id = 4294967295;
    c->hp = 1800;
    c->r_weapon_id = 301015;
    c->l_weapon_id = 900000;
    c->subanimation = SubanimationNeutral;
    c->velocity = 0;
}