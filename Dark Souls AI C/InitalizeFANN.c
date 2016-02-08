//program to train, teach, and create neural net
#pragma warning(disable: 4244)
#define _CRT_SECURE_NO_WARNINGS

#include "Settings.h"
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

#define RRAND(min,max) (min + rand() % (max - min))

FILE* fpdef;
FILE* fpatk;
Character* TwoSecStore[20];
static long lastCopyTime = 0;
static long lastBsCheckTime = 0;

static const int Player_AnimationId3_offsets[] = { 0x3C, 0x10C };
static const int Player_AnimationId3_offsets_length = 2;
ullong AnimationId3_Addr;
int AnimationId3;
static const int Player_Timer3_offsets[] = { 0x3C, 0x28, 0x18, 0x7DC, 0x98 };
static const int Player_Timer3_offsets_length = 5;
ullong Timer3_Addr;
float Timer3;


void GetTrainingData(){
    #if DisableAi
    ReadPlayer(&Enemy, processHandle, EnemyId);
    ReadPlayer(&Player, processHandle, PlayerId);
    AppendDistance(distance(&Player, &Enemy));
    #else
    MainLogicLoop();
    #endif
    ReadProcessMemory(processHandle, (LPCVOID)(AnimationId3_Addr), &(AnimationId3), 4, 0);
    ReadProcessMemory(processHandle, (LPCVOID)(Timer3_Addr), &(Timer3), 4, 0);

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
    if (isAttackAnimation(Player.animationType_id) && DistanceMemory[49] != 0){
        unsigned int startingHp = Player.hp;
        unsigned int startingHpEnemy = Enemy.hp;

        //output the array of distance values
        for (int i = 0; i < DistanceMemoryLENGTH; i++){
            fprintf(fpatk, "%f ", DistanceMemory[i]);
        }
        //output estimated stamina of enemy
        fprintf(fpatk, "%f ", (float)StaminaEstimationEnemy());
        //output the enemy's current poise
        fprintf(fpatk, "%f ", Enemy.poise);
        //output the AI's attack's poise damage (just r1 for now)
        fprintf(fpatk, "%f ", PoiseDamageForAttack(Player.r_weapon_id, 46));
        //output the AI's current poise
        fprintf(fpatk, "%f ", Player.poise);
        //base poise damage of enemy's attack (treat r1 as base)
        fprintf(fpatk, "%f ", PoiseDamageForAttack(Enemy.r_weapon_id, 46));
        //output array of AI's HP over time
        for (int i = 0; i < AIHPMemoryLENGTH; i++){
            fprintf(fpatk, "%f ", (float)AIHPMemory[i]);
        }
        //stamina of AI
        fprintf(fpatk, "%f ", (float)Player.stamina);
        //output array of enemy animation types
        for (int i = 0; i < last_animation_types_enemy_LENGTH; i++){
            fprintf(fpatk, "%f ", (float)last_animation_types_enemy[i]);
        }
        //current bleed built up
        fprintf(fpatk, "%f ", (float)Player.bleedStatus);

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
        //neutral outcome
        else if (startingHp == Player.hp && startingHpEnemy == Enemy.hp){
            result = 0;
        }
        //good outcome
        else if (startingHp == Player.hp && startingHpEnemy != Enemy.hp){
            result = 1;
        }

        //output result
        fprintf(fpatk, "\n%f\n", result);

        printf("Attack result:%f\n", result);

        unsigned int resethp = 2000;

        //reset hp so we dont die
        //WriteProcessMemory(processHandle, (LPVOID)Player.hp_address, &resethp, 4, 0);
    }

    bool backstabCheckTime = clock() - lastBsCheckTime > 3000;// RRAND(2500, 4000);

    //player in backstab state when animation id 3 is 9000, 9420
    if ((((AnimationId3 == 9000 || AnimationId3 == 9420) && (Timer3 < 0.1 && Timer3 > 0)) || (rand() < 100 && backstabCheckTime)) && TwoSecStore[19] != NULL){
        //output the array of distance values
        for (int i = 0; i < DistanceMemoryLENGTH; i++){
            fprintf(fpdef, "%f ", DistanceMemory[i]);
        }

        fprintf(fpdef, "%f %f %f\n%f\n",
            angleDeltaFromFront(TwoSecStore[18], TwoSecStore[19]),
            TwoSecStore[19]->velocity,
            rotationDifferenceFromSelf(TwoSecStore[18], TwoSecStore[19]),
            (Enemy.animationType_id == Backstab ? 1.0 : -1.0)
            );

        printf("BackStab result:%d\n", Enemy.animationType_id == Backstab);
        Sleep(100);
    }

    if (backstabCheckTime){
        backstabCheckTime = clock();
    }
}

//use the file to train the network
void trainFromFile(unsigned int max_neurons,const char* training_file, const char* output_file){
    struct fann *ann;
    struct fann_train_data *train_data;
    const float desired_error = (const float)0.05;
    unsigned int neurons_between_reports = 5;
    fann_type steepness;
    int multi = 0;
    enum fann_activationfunc_enum activation;
    enum fann_train_enum training_algorithm = FANN_TRAIN_RPROP;

    printf("Reading data.\n");

    train_data = fann_read_train_from_file(training_file);

    fann_scale_train_data(train_data, -1, 1);

    printf("Creating network.\n");
    printf("input number:%d, output number:%d\n", fann_num_input_train_data(train_data), fann_num_output_train_data(train_data));

    ann = fann_create_shortcut(2, fann_num_input_train_data(train_data), fann_num_output_train_data(train_data));

    fann_set_training_algorithm(ann, training_algorithm);
    fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
    fann_set_activation_function_output(ann, FANN_LINEAR);
    fann_set_train_error_function(ann, FANN_ERRORFUNC_LINEAR);

    if (!multi)
    {
        /*steepness = 0.5;*/
        steepness = 1;
        fann_set_cascade_activation_steepnesses(ann, &steepness, 1);
        /*activation = FANN_SIN_SYMMETRIC;*/
        activation = FANN_SIGMOID_SYMMETRIC;

        fann_set_cascade_activation_functions(ann, &activation, 1);
        fann_set_cascade_num_candidate_groups(ann, 8);
    }

    if (training_algorithm == FANN_TRAIN_QUICKPROP)
    {
        fann_set_learning_rate(ann, 0.35f);
        fann_randomize_weights(ann, -2.0f, 2.0f);
    }

    fann_set_bit_fail_limit(ann, (fann_type)0.9);
    fann_set_train_stop_function(ann, FANN_STOPFUNC_BIT);
    fann_print_parameters(ann);

    printf("Training network.\n");
    fann_cascadetrain_on_data(ann, train_data, max_neurons, neurons_between_reports, desired_error);
    fann_print_connections(ann);

    printf("Saving network.\n");
    fann_save(ann, output_file);

    printf("Cleaning up.\n");
    fann_destroy_train(train_data);
    fann_destroy(ann);
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
    fpatk = fopen("Neural Nets/attack_training_data.train", "a");
    fpdef = fopen("Neural Nets/backstab_training_data.train", "a");
    AnimationId3_Addr = FindPointerAddr(processHandle, player_base_add, Player_AnimationId3_offsets_length, Player_AnimationId3_offsets);
    Timer3_Addr = FindPointerAddr(processHandle, player_base_add, Player_Timer3_offsets_length, Player_Timer3_offsets);
}