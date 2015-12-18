#include "CharacterStruct.h"
#define PI 3.14159265

#pragma warning( disable: 4244 )//ignore dataloss conversion from double to float

void ReadPlayer(Character * c, HANDLE * processHandle, int Character){
	HANDLE processHandle_nonPoint = *processHandle;
    //TODO read large block that contains all data, then parse in process
	//read x location
	ReadProcessMemory(processHandle_nonPoint, (LPCVOID)(c->location_x_address), &(c->loc_x), 4, 0);
    guiPrint("%d,0:X:%f", Character, c->loc_x);
	//read y location
	ReadProcessMemory(processHandle_nonPoint, (LPCVOID)(c->location_y_address), &(c->loc_y), 4, 0);
    guiPrint("%d,1:Y:%f", Character, c->loc_y);
	//read rotation of player
	ReadProcessMemory(processHandle_nonPoint, (LPCVOID)(c->rotation_address), &(c->rotation), 4, 0);
	//Player rotation is pi. 0 to pi,-pi to 0. Same as atan2
	//convert to radians, then to degrees
	c->rotation = (c->rotation + PI) * (180.0 / PI);
    guiPrint("%d,2:Rotation:%f", Character, c->rotation);
	//read current animation id
	ReadProcessMemory(processHandle_nonPoint, (LPCVOID)(c->animation_address), &(c->animation_id), 2, 0);
    guiPrint("%d,3:Animation Id:%d", Character, c->animation_id);
	//read hp
	ReadProcessMemory(processHandle_nonPoint, (LPCVOID)(c->hp_address), &(c->hp), 4, 0);
    //read stamina
    if (c->stamina_address){
        ReadProcessMemory(processHandle_nonPoint, (LPCVOID)(c->stamina_address), &(c->stamina), 4, 0);
        guiPrint("%d,4:Stamina:%d", Character, c->stamina);
    }
	//read what weapon they currently have in right hand
	ReadProcessMemory(processHandle_nonPoint, (LPCVOID)(c->r_weapon_address), &(c->r_weapon_id), 4, 0);
    guiPrint("%d,5:R Weapon:%d", Character, c->r_weapon_id);
	//read what weapon they currently have in left hand
	ReadProcessMemory(processHandle_nonPoint, (LPCVOID)(c->l_weapon_address), &(c->l_weapon_id), 4, 0);
    guiPrint("%d,6:L Weapon:%d", Character, c->l_weapon_id);

	//read if animation in windup
    if (c->windup_address){
        ReadProcessMemory(processHandle_nonPoint, (LPCVOID)(c->windup_address), &(c->subanimation), 1, 0);
    }
	//read if hurtbox is active on enemy weapon
    if (c->hurtboxActive_address){
        unsigned char hurtboxActiveState;
        ReadProcessMemory(processHandle_nonPoint, (LPCVOID)(c->hurtboxActive_address), &hurtboxActiveState, 1, 0);
        if (hurtboxActiveState){
            c->subanimation = AttackSubanimationActiveDuringHurtbox;
        }
    }
    //read how long the animation has been active, check with current animation, see if hurtbox is about to activate
    //what i want is a countdown till hurtbox is active
    if (c->animationTimer_address){
        int attackAnimationid;
        ReadProcessMemory(processHandle_nonPoint, (LPCVOID)(c->attackAnimationId_address), &attackAnimationid, 4, 0);
        guiPrint("%d,7:Attack Id:%d", Character, attackAnimationid);

        if (isVulnerableAnimation(attackAnimationid))
        {
            c->subanimation = AttackSubanimationActiveHurtboxOver;
        }
        //cant be much higher b/c need spell attack timings
        //also check that this is an attack that involves subanimation
        else if (attackAnimationid > 1000 && isAttackAnimation(c->animation_id) == 2){
            //if kick or parry, immediate dodge away (aid ends in 100)
            if (attackAnimationid % 1000 == 100){
                c->subanimation = AttackSubanimationWindupClosing;
            }
            else{
                float animationTimer;
                ReadProcessMemory(processHandle_nonPoint, (LPCVOID)(c->animationTimer_address), &animationTimer, 4, 0);
                float dodgeTimer = dodgeTimings(attackAnimationid);
                //only bother to set this for a specific range
                if ((animationTimer <= dodgeTimer - 0.2) && (animationTimer + 0.4 >= dodgeTimer)){
                    c->subanimation = AttackSubanimationWindupClosing;
                }
            }
        }
        else{
            c->subanimation = SubanimationNeutral;
        }
    }
    //read if in recovery subanimation(can transition to another animation)
    if (c->recoveryState_address){
        unsigned char recoveryState;
        ReadProcessMemory(processHandle_nonPoint, (LPCVOID)(c->recoveryState_address), &recoveryState, 1, 0);
        if (recoveryState){
            c->subanimation = AttackSubanimationRecover;
        }
    }
    guiPrint("%d,8:Subanimation:%d", Character, c->subanimation);

    //read the current velocity
    //player doesnt use this, and wont have the address set. enemy will
    if (c->velocity_address){
        ReadProcessMemory(processHandle_nonPoint, (LPCVOID)(c->velocity_address), &(c->velocity), 4, 0);
        guiPrint("%d,9:Velocity:%f", Character, c->velocity);
    }
    //read if the player is locked on
    if (c->locked_on_address){
        ReadProcessMemory(processHandle_nonPoint, (LPCVOID)(c->locked_on_address), &(c->locked_on), 1, 0);
        guiPrint("%d,10:Locked On:%d", Character, c->locked_on);
    }
}

void ReadPlayerDEBUGGING(Character * c, HANDLE * processHandle, ...){
    c->loc_x = 1045.967773;
    c->loc_y = 864.3547974;
    c->rotation = 360;//facing kinda towards bonfire, same as pi/-pi
    c->animation_id = 4294967295;
    c->hp = 1800;
    c->r_weapon_id = 301015;
    c->l_weapon_id = 900000;
    c->subanimation = SubanimationNeutral;
    c->velocity = 0;
}