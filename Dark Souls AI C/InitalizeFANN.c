//program to train, teach, and create neural net
#pragma warning(disable: 4244)
#define _CRT_SECURE_NO_WARNINGS

#include "Source.h"
#include "MemoryEdits.h"
#include "CharacterStruct.h"
#include "AnimationMappings.h"
#include "gui.h"
#include "fann.h"

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

FILE* fpdef;
FILE* fpatk;
Character* TwoSecStore[20];
static long lastCopyTime = 0;

void GetTrainingData(){
    MainLogicLoop();

    //store copy of player and enemy structs every 100 ms for 2 sec
    if (clock() - lastCopyTime > 100){
        free(TwoSecStore[19]);
        free(TwoSecStore[18]);
        for (unsigned int i = 19; i > 1; i--){
            TwoSecStore[i] = TwoSecStore[i - 2];
        }
        TwoSecStore[1] = (Character*)malloc(sizeof(Character));
        memcpy(TwoSecStore[1], &Enemy, sizeof(Character));
        TwoSecStore[0] = malloc(sizeof(Character));
        memcpy(TwoSecStore[0], &Player, sizeof(Character));

        lastCopyTime = clock();
    }

    //have random attacks. if it doesnt get hit, sucess. if it gets hit, fail.
    if (subroutine_states[AttackTypeIndex] == GhostHitId){
        unsigned int startingHp = Player.hp;
        float startingPoiseAI = Player.poise;
        float startingPoiseEnemy = Enemy.poise;

        //output the array of distance values
        for (int i = 0; i < DistanceMemoryLENGTH; i++){
            fprintf(fpatk, "%f ", DistanceMemory[i]);
        }
        //output estimated stamina
        fprintf(fpatk, "%f ", (float)StaminaEstimationEnemy());
        //output the enemy's current poise
        fprintf(fpatk, "%f ", startingPoiseEnemy);
        //output the AI's attack's poise damage (just r1 for now)
        fprintf(fpatk, "%f ", PoiseDamageForAttack(Player.r_weapon_id, 46));
        //output the AI's current poise
        fprintf(fpatk, "%f ", startingPoiseAI);
        //base poise damage of enemy's attack (treat r1 as base)
        fprintf(fpatk, "%f ", PoiseDamageForAttack(Enemy.r_weapon_id, 46));
        //output array of AI's HP over time
        for (int i = 0; i < AIHPMemoryLENGTH; i++){
            fprintf(fpatk, "%f ", (float)AIHPMemory[i]);
        }
        //output array of AI's subanimations
        for (int i = 0; i < last_subroutine_states_self_LENGTH; i++){
            fprintf(fpatk, "%f ", (float)last_subroutine_states_self[i]);
        }

        //2 seconds
        long startTime = clock();
        while (clock() - startTime < 2000){
            MainLogicLoop();
        }

        float result = 0;
        //bad outcome
        if (startingHp != Player.hp){
            result = -1;
        }
        //good outcome
        else{
            result = 1;
        }

        //output result
        fprintf(fpatk, "%f\n", result);

        printf("Attack result:%f\n", result);

        unsigned int resethp = 2000;

        //reset hp so we dont die
        //WriteProcessMemory(processHandle, (LPVOID)Player.hp_address, &resethp, 4, 0);
    }

    //player in backstab or random positive data
    if ((Player.animationType_id == 108 || (rand() < 5 && clock() - lastCopyTime > 70)) && TwoSecStore[19] != NULL){

        //output the array of distance values
        for (int i = 0; i < DistanceMemoryLENGTH; i++){
            fprintf(fpdef, "%f ", DistanceMemory[i]);
        }

        fprintf(fpdef, "%f %f %f %f\n",
            angleDeltaFromFront(TwoSecStore[18], TwoSecStore[19]),
            TwoSecStore[19]->velocity,
            rotationDifferenceFromSelf(TwoSecStore[18], TwoSecStore[19]),
            (Player.animationType_id == 108 ? 1.0 : -1.0)
            );

        printf("BackStab result:%f\n", Player.animationType_id == 108);
    }
}

//use the file to train the network
int trainFromFile(void){
    //create new, empty network
    struct fann *net = fann_create_shortcut(2, 95, 1);//set up net without hidden layers. N inputs, 1 output
    fann_set_training_algorithm(net, FANN_TRAIN_RPROP);
    fann_set_activation_function_hidden(net, FANN_SIGMOID_SYMMETRIC);
    fann_set_activation_function_output(net, FANN_LINEAR);
    fann_set_train_error_function(net, FANN_ERRORFUNC_LINEAR);
    fann_set_bit_fail_limit(net, (fann_type)0.9);
    fann_set_train_stop_function(net, FANN_STOPFUNC_BIT);
    fann_print_parameters(net);

    //load training data
    unsigned int max_neurons = 100;
    const float desired_error = (const float)0.05;

    struct fann_train_data *data = fann_read_train_from_file("E:/Code Workspace/Dark Souls AI C/Neural Nets/attack_training_data.train");

    fann_scale_train_data(data, -1, 1);

    //train network
    fann_cascadetrain_on_data(net, data, max_neurons, 0, desired_error);

    //TODO test trained network on test data

    //save and clean up
    fann_print_connections(net);

    fann_save(net, "E:/Code Workspace/Dark Souls AI C/Neural Nets/Attack_dark_souls_ai.net");

    fann_destroy_train(data);
    fann_destroy(net);

    return 0;
}

/*
void testData(void){
    struct fann* defense_mind = fann_create_from_file("E:/Code Workspace/Dark Souls AI C/Neural Nets/backstab_train.net");

    memset(&Enemy, 0, sizeof(Character));

    HANDLE processHandle = readingSetup();

    while (true){
        long start = clock();

        CharState* enemy = ReadPlayerFANN(&Enemy, processHandle);
        CharState* player = ReadPlayerFANN(&Player, processHandle);

        //read inputs and scale to -1 - 1
        float distance = distanceFANN(player, enemy);
        distance = 2 * (distance - 0.3) / (5 - 0.3) - 1;

        float angleDelta = angleDeltaFromFrontFANN(player, enemy);
        angleDelta = 2 * (angleDelta) / (1.6) - 1;

        float velocity = enemy->velocity;
        velocity = 2 * (velocity - -0.18) / (-0.04 - -0.18) - 1;

        float rotationDelta = rotationDifferenceFromSelfFANN(player, enemy);//rotation with respect to self rotation
        rotationDelta = 2 * (rotationDelta) / (3.8) - 1;

        float input[4] = { distance, angleDelta, velocity, rotationDelta };
        fann_type* out = fann_run(defense_mind, input);

        if (*out < 1.5 && *out > 0.5){
            printf("%f ", *out);
        }

        printf("%d\n", clock() - start);
    }

}
*/

void SetupTraining(){
    fpatk = fopen("E:/Code Workspace/Dark Souls AI C/Neural Nets/attack_training_data.train", "a");
    fpdef = fopen("E:/Code Workspace/Dark Souls AI C/Neural Nets/backstab_training_data.train", "a");
    fprintf(fpatk, "X %d 1\n", DistanceMemoryLENGTH + 1 + 1 + 1 + 1 + 1 + AIHPMemoryLENGTH + last_subroutine_states_self_LENGTH);
    fprintf(fpdef, "X %d 1\n", DistanceMemoryLENGTH+3);
}