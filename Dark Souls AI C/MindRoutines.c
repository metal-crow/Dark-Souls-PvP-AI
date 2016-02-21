#include "MindRoutines.h"
#pragma warning( disable: 4244 )

#define SCALE(input, minVal, maxVal) (2 * ((float)input - minVal) / (maxVal - minVal) - 1)


DWORD WINAPI DefenseMindProcess(void* data){
    while (!defense_mind_input->exit)
    {
        //lock control of this resource
        EnterCriticalSection(&(defense_mind_input->crit));
        //wait for the indicator this should run, and release lock in meantime
        while (defense_mind_input->runNetwork == false){
            SleepConditionVariableCS(&(defense_mind_input->cond), &(defense_mind_input->crit), INFINITE);
        }

        //generate inputs and scale from -1 to 1 
        fann_type input[8];

        //copy inputs into input and scale
        float mostRecentDistance = distance(&Player, &Enemy);
        input[0] = SCALE(mostRecentDistance, 0, 10);
        input[0] = input[0] > 1 ? 1 : input[0];
        input[0] = input[0] < -1 ? -1 : input[0];
        for (int i = 0; i < 4; i++){
            input[i+1] = SCALE(DistanceMemory[i], 0, 10);
            //cut off above and below
            input[i+1] = input[i+1] > 1 ? 1 : input[i+1];
            input[i+1] = input[i+1] < -1 ? -1 : input[i+1];
        }
        input[5] = SCALE(angleDeltaFromFront(&Player, &Enemy), 0, 1.6);
        input[6] = SCALE(Enemy.velocity, -0.18, -0.04);
        input[7] = SCALE(rotationDifferenceFromSelf(&Player, &Enemy), 0, 3.8);

        fann_type* out = fann_run(defense_mind_input->mind, input);
        //printf("%f\n", *out);
        if (*out < 10 && *out > 0.5
            && mostRecentDistance < 5){//hardcode bs distance
            DefenseChoice = CounterStrafeId;
        } 
        //hardcoded check if the enemy is close behind us, try to damage cancel their bs. TEMP: this is a bandaid and should not be permenant
        if (mostRecentDistance < 2 && BackstabDetection(&Enemy, &Player, mostRecentDistance)){
            AttackChoice = GhostHitId;
        }
        //if we had to toggle escape, they're probably comboing. Get out.
        if (last_subroutine_states_self[0] == ToggleEscapeId){
            DefenseChoice = StandardRollId;
        }

        //prevent rerun
        defense_mind_input->runNetwork = false;
        //release lock
        LeaveCriticalSection(&(defense_mind_input->crit));
        WakeConditionVariable(&(defense_mind_input->cond));
    }
    return 0;
}

DWORD WINAPI AttackMindProcess(void* data){
    while (!attack_mind_input->exit)
    {
        //lock control of this resource
        EnterCriticalSection(&(attack_mind_input->crit));
        //wait for the indicator this should run, and release lock in meantime
        while (attack_mind_input->runNetwork == false){
            SleepConditionVariableCS(&(attack_mind_input->cond), &(attack_mind_input->crit), INFINITE);
        }

        //generate inputs and scale from -1 to 1 
        fann_type input[DistanceMemoryLENGTH + 5 + AIHPMemoryLENGTH + 1 + last_animation_types_enemy_LENGTH + 1];

        //copy inputs into input and scale
        float mostRecentDistance = distance(&Player, &Enemy);
        input[0] = SCALE(mostRecentDistance, 0, 10);
        input[0] = input[0] > 1 ? 1 : input[0];
        input[0] = input[0] < -1 ? -1 : input[0];
        for (int i = 0; i < DistanceMemoryLENGTH-1; i++){
            input[i+1] = SCALE(DistanceMemory[i], 0, 10);
            //cut off above and below
            input[i+1] = input[i+1] > 1 ? 1 : input[i+1];
            input[i+1] = input[i+1] < -1 ? -1 : input[i+1];
        }
        input[DistanceMemoryLENGTH] = SCALE(StaminaEstimationEnemy(), -40, 192);
        input[DistanceMemoryLENGTH + 1] = SCALE(Enemy.poise, 0, 120);
        input[DistanceMemoryLENGTH + 2] = SCALE(PoiseDamageForAttack(Player.r_weapon_id, 46), 0, 80);
        input[DistanceMemoryLENGTH + 3] = SCALE(Player.poise, 0, 120);
        input[DistanceMemoryLENGTH + 4] = SCALE(PoiseDamageForAttack(Enemy.r_weapon_id, 46), 0, 80);
        for (int i = 0; i < AIHPMemoryLENGTH; i++){
            input[i + DistanceMemoryLENGTH + 5] = SCALE(AIHPMemory[i], 0, 2000);
        }
        input[DistanceMemoryLENGTH + 5 + AIHPMemoryLENGTH] = SCALE(Player.stamina, -40, 192);
        for (int i = 0; i < last_animation_types_enemy_LENGTH; i++){
            input[i + DistanceMemoryLENGTH + 5 + AIHPMemoryLENGTH + 1] = SCALE(last_animation_types_enemy[i], 0, 255);
        }
        input[DistanceMemoryLENGTH + 5 + AIHPMemoryLENGTH + 1 + last_animation_types_enemy_LENGTH] = SCALE(Player.bleedStatus, 0, 255);

        fann_type* out = fann_run(attack_mind_input->mind, input);

        if (
            //not in range
            mostRecentDistance > Player.weaponRange ||
            //we're behind the enemy and might be able to get a backstab
            BackstabDetection(&Player, &Enemy, mostRecentDistance) == 1)
        {
            AttackChoice = MoveUpId;
        }
        //if not two handing
        if (!Player.twoHanding && mostRecentDistance > 5){
            AttackChoice = TwoHandId;
        }
        //l hand bare handed, not holding shield. safety distance
        if (Player.l_weapon_id == 900000 && mostRecentDistance > 5){
            AttackChoice = SwitchWeaponId;
        }
        if (
            //sanity checks
            mostRecentDistance <= Player.weaponRange && //in range
            Player.stamina > 20 && //just to ensure we have enough to roll
            Player.bleedStatus > 40 && //more than one attack to proc bleed
            //static checks for attack
            ((
                (Player.stamina > 90) && //safety buffer for stamina
                (Enemy.subanimation >= LockInSubanimation && Enemy.subanimation < SubanimationNeutral)  //enemy in vulnerable state, and can't immediatly transition
            ) ||
            (*out > 0.5)//neural network says so
           ))
        {
            //randomly choose dead angle or ghost hit
            //throw off enemy predictions
            if (rand() > RAND_MAX / 2){
                AttackChoice = DeadAngleId;
            }
            else{
                AttackChoice = GhostHitId;
            }
        }
        if ((Enemy.animationType_id == CrushUseItem || Enemy.animationType_id == EstusSwig_part1 || Enemy.animationType_id == EstusSwig_part2) && Player.hp < 2000){
            AttackChoice = HealId;
        }

        //prevent rerun
        attack_mind_input->runNetwork = false;
        //release lock
        LeaveCriticalSection(&(attack_mind_input->crit));
        WakeConditionVariable(&(attack_mind_input->cond));
    }
    return 0;
}

//Helper Methods

int ReadyThreads(){
    //Defense Thread
    defense_mind_input = malloc(sizeof(MindInput));
    struct fann* defense_mind = fann_create_from_file("Neural Nets/Defense_dark_souls_ai.net");
    if (defense_mind == NULL){
        printf("Defense_dark_souls_ai.net neural network file not found");
        return EXIT_FAILURE;
    }
    defense_mind_input->mind = defense_mind;
    defense_mind_input->exit = false;
    InitializeConditionVariable(&(defense_mind_input->cond));
    InitializeCriticalSection(&(defense_mind_input->crit));
    EnterCriticalSection(&(defense_mind_input->crit));
    defense_mind_input->runNetwork = false;
    HANDLE* defense_mind_thread = CreateThread(NULL, 0, DefenseMindProcess, NULL, 0, NULL);

    //Attack Thread
    attack_mind_input = malloc(sizeof(MindInput));
    struct fann* attack_mind = fann_create_from_file("Neural Nets/Attack_dark_souls_ai.net");
    if (attack_mind == NULL){
        printf("Attack_dark_souls_ai.net neural network file not found");
        return EXIT_FAILURE;
    }
    attack_mind_input->mind = attack_mind;
    attack_mind_input->exit = false;
    InitializeConditionVariable(&(attack_mind_input->cond));
    InitializeCriticalSection(&(attack_mind_input->crit));
    EnterCriticalSection(&(attack_mind_input->crit));
    attack_mind_input->runNetwork = false;
    HANDLE* attack_mind_thread = CreateThread(NULL, 0, AttackMindProcess, NULL, 0, NULL);

    return 0;
}

void WaitForThread(MindInput* input){
    //get control of lock
    EnterCriticalSection(&(input->crit));
    //wait for neural net thread to mark self as finished
    while (input->runNetwork == true){
        bool result = SleepConditionVariableCS(&(input->cond), &(input->crit), 10);
        if (!result){
            guiPrint(LocationDetection",2:Timeout in reaquiring thread");
            break;
        }
    }
}

void WakeThread(MindInput* input){
    //trigger threads to run
    input->runNetwork = true;
    //release lock
    LeaveCriticalSection(&(input->crit));
    //wake thread
    WakeConditionVariable(&(input->cond));
}