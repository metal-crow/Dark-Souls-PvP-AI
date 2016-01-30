#include "MindRoutines.h"
#pragma warning( disable: 4244 )

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
        fann_type input[4];

        float distanceInput = distance(&Player, &Enemy);
        //min:0.3 max:5
        input[0] = 2 * (distanceInput - 0.3) / (5 - 0.3) - 1;

        float angleDeltaInput = angleDeltaFromFront(&Player, &Enemy);
        //min:0 max:1.6
        input[1] = 2 * (angleDeltaInput) / (1.6) - 1;

        //min:-0.18 max:-0.04
        input[2] = 2 * (Enemy.velocity - -0.18) / (-0.04 - -0.18) - 1;

        float rotationDeltaInput = rotationDifferenceFromSelf(&Player, &Enemy);
        //min:0 max:3.8
        input[3] = 2 * (rotationDeltaInput) / (3.8) - 1;


        fann_type* out = fann_run(defense_mind_input->mind, input);
        if (*out < 1.5 && *out > 0.5
            && distanceInput < 5){//hardcode bs distance
            DefenseChoice = CounterStrafeId;
        } 
        //hardcoded check if the enemy is close behind us, try to damage cancel their bs. TEMP: this is a bandaid and should not be permenant
        if (distanceInput < 2 && BackstabDetection(&Enemy, &Player, distanceInput)){
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
        fann_type input[DistanceMemoryLENGTH+1];

        //copy distances into input and scale
        for (int i = 0; i < DistanceMemoryLENGTH; i++){
            //min:0 max:10
            input[i] = 2 * (DistanceMemory[i] / 10.0) - 1;
            //cut off above and below
            input[i] = input[i] > 1 ? 1 : input[i];
            input[i] = input[i] < -1 ? -1 : input[i];
        }
        //scale stamina estimate min:-40 max:192
        input[DistanceMemoryLENGTH] = 2 * (StaminaEstimationEnemy() - -40) / (192 - -40) - 1;

        fann_type* out = fann_run(attack_mind_input->mind, input);

        if (
            //not in range
            DistanceMemory[0] > Player.weaponRange ||
            //we're behind the enemy and might be able to get a backstab
            BackstabDetection(&Player, &Enemy, DistanceMemory[0]) == 1)
        {
            AttackChoice = MoveUpId;
        }
        if (
            (Player.stamina > 90) &&  //TODO NEural net handles
            (Enemy.subanimation >= LockInSubanimation) &&  //enemy in vulnerable state
            DistanceMemory[0] <= Player.weaponRange &&  //in range
            (
                (*out > 0) || 
                Enemy.animationType_id == EstusSwig_part1 || Enemy.animationType_id == EstusSwig_part2 || Enemy.animationType_id == EstusSwig_part3 || Enemy.animationType_id == UseItem
            ) //neural network says so, or enemy is super weak
           ){
            AttackChoice = GhostHitId;
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