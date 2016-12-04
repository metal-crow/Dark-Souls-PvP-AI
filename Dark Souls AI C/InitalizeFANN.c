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

FILE *fpdef, *fpdef_test, *fpatk, *fpatk_test;
#define TwoSecStoreLength 40
Character* TwoSecStore[TwoSecStoreLength];
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

    //store copy of player and enemy structs every 100 ms for 3.5 sec
    if (clock() - lastCopyTime > 100){
        free(TwoSecStore[TwoSecStoreLength-1]);
        free(TwoSecStore[TwoSecStoreLength-2]);
        for (unsigned int i = TwoSecStoreLength-1; i > 1; i--){
            TwoSecStore[i] = TwoSecStore[i - 2];
        }
        TwoSecStore[1] = (Character*)malloc(sizeof(Character));
        memcpy(TwoSecStore[1], &Enemy, sizeof(Character));
        TwoSecStore[0] = malloc(sizeof(Character));
        memcpy(TwoSecStore[0], &Player, sizeof(Character));

        lastCopyTime = clock();
    }

    //have random attacks. if it doesnt get hit, sucess. if it gets hit, fail.
    if (isAttackAnimation(Player.animationType_id) && DistanceMemory[DistanceMemoryLENGTH-1] != 0 && TrainAttackNet){
        FILE* outFile = rand() < RAND_MAX / 3 ? fpatk_test : fpatk;

        unsigned int startingHp = Player.hp;
        unsigned int startingHpEnemy = Enemy.hp;

        //output the array of distance values
        for (int i = 0; i < DistanceMemoryLENGTH; i++){
            fprintf(outFile, "%f ", DistanceMemory[i]);
        }
        //output estimated stamina of enemy
        fprintf(outFile, "%f ", (float)StaminaEstimationEnemy());
        //output the enemy's current poise
        fprintf(outFile, "%f ", Enemy.poise);
        //output the AI's attack's poise damage (just r1 for now)
        fprintf(outFile, "%f ", PoiseDamageForAttack(Player.r_weapon_id, 46));
        //output the AI's current poise
        fprintf(outFile, "%f ", Player.poise);
        //base poise damage of enemy's attack (treat r1 as base)
        fprintf(outFile, "%f ", PoiseDamageForAttack(Enemy.r_weapon_id, 46));
        //output array of AI's HP over time
        for (int i = 0; i < AIHPMemoryLENGTH; i++){
            fprintf(outFile, "%f ", (float)AIHPMemory[i]);
        }
        //stamina of AI
        fprintf(outFile, "%f ", (float)Player.stamina);
        //output array of enemy animation types
        for (int i = 0; i < last_animation_types_enemy_LENGTH; i++){
            fprintf(outFile, "%f ", (float)last_animation_types_enemy[i]);
        }
        //current bleed built up
        fprintf(outFile, "%f ", (float)Player.bleedStatus);

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
        fprintf(outFile, "\n%f\n", result);
		fflush(outFile);
        printf("Attack result:%f in %s\n", result, (outFile == fpatk_test ? "Test" : "Train"));

        unsigned int resethp = 2000;

        //reset hp so we dont die
        //WriteProcessMemory(processHandle, (LPVOID)Player.hp_address, &resethp, 4, 0);
    }

    bool backstabCheckTime = clock() - lastBsCheckTime > 3000;// RRAND(2500, 4000);

    //player in backstab state when animation id 3 is 9000, 9420
    if ((((AnimationId3 == 9000 || AnimationId3 == 9420) && (Timer3 < 0.1 && Timer3 > 0)) || (rand() < 1000 && backstabCheckTime)) && TwoSecStore[TwoSecStoreLength-1] != NULL && TrainBackstabNet){
        FILE* outFile = rand() < RAND_MAX / 3 ? fpdef_test : fpdef;

        //output an array of 5 distance values from 3500 ms ago
        for (int i = TwoSecStoreLength - 5; i < TwoSecStoreLength; i++){
            fprintf(outFile, "%f ", DistanceMemory[i]);
        }

        fprintf(outFile, "%f %f %f\n%f\n",
            angleDeltaFromFront(TwoSecStore[TwoSecStoreLength-6], TwoSecStore[TwoSecStoreLength-7]),
            TwoSecStore[TwoSecStoreLength-7]->velocity,
            rotationDifferenceFromSelf(TwoSecStore[TwoSecStoreLength-6], TwoSecStore[TwoSecStoreLength-7]),
            ((AnimationId3 == 9000 || AnimationId3 == 9420) ? 1.0 : -1.0)
            );
		fflush(outFile);
        printf("BackStab result:%d in %s\n", ((AnimationId3 == 9000 || AnimationId3 == 9420) ? 1 : -1), (outFile == fpdef_test ? "Test" : "Train"));
        Sleep(100);
    }

    if (backstabCheckTime){
        lastBsCheckTime = clock();
    }
}

//use the file to train the network
void trainFromFile(unsigned int max_neurons, const char* training_file, const char* testing_file, const char* output_file){
    struct fann *ann;
    struct fann_train_data *train_data, *test_data;
    const float desired_error = (const float)0.05;
    unsigned int neurons_between_reports = 5;

    printf("Reading data.\n");

    train_data = fann_read_train_from_file(training_file);
    test_data = fann_read_train_from_file(testing_file);

    fann_scale_train_data(train_data, -1, 1);

    printf("Creating network.\n");
    printf("input number:%d, output number:%d\n", fann_num_input_train_data(train_data), fann_num_output_train_data(train_data));

    ann = fann_create_shortcut(2, fann_num_input_train_data(train_data), fann_num_output_train_data(train_data));

    fann_set_training_algorithm(ann, FANN_TRAIN_RPROP);
    fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
    fann_set_activation_function_output(ann, FANN_LINEAR);
    fann_set_train_error_function(ann, FANN_ERRORFUNC_LINEAR);
    fann_set_bit_fail_limit(ann, (fann_type)0.9);
    fann_set_train_stop_function(ann, FANN_STOPFUNC_BIT);
    fann_print_parameters(ann);

    printf("Training network.\n");
    fann_cascadetrain_on_data(ann, train_data, max_neurons, neurons_between_reports, desired_error);
    fann_print_connections(ann);

    float mse_train = fann_test_data(ann, train_data);
    unsigned int bit_fail_train = fann_get_bit_fail(ann);
    float mse_test = fann_test_data(ann, test_data);
    unsigned int bit_fail_test = fann_get_bit_fail(ann);

    printf("\nTrain error: %f, Train bit-fail: %d, Test error: %f, Test bit-fail: %d\n\n",
        mse_train, bit_fail_train, mse_test, bit_fail_test);

    for (unsigned int i = 0; i < train_data->num_data; i++)
    {
        fann_type* output = fann_run(ann, train_data->input[i]);
        if (abs(train_data->output[i][0] - output[0]) > 5 || train_data->output[i][0] >= 0 && output[0] <= 0 || train_data->output[i][0] <= 0 && output[0] >= 0)
        {
            printf("ERROR: %f does not match %f\n", train_data->output[i][0], output[0]);
        }
    }


    printf("Saving network.\n");
    fann_save(ann, output_file);

    printf("Cleaning up.\n");
    fann_destroy_train(train_data);
    fann_destroy(ann);
}


void SetupTraining(){
	fpatk = fopen(NeuralNetFolderLocation"/attack_training_data.train", "a");
	fpatk_test = fopen(NeuralNetFolderLocation"/attack_training_data.test", "a");
	fpdef = fopen(NeuralNetFolderLocation"/backstab_training_data.train", "a");
	fpdef_test = fopen(NeuralNetFolderLocation"/backstab_training_data.test", "a");
    AnimationId3_Addr = FindPointerAddr(processHandle, player_base_add, Player_AnimationId3_offsets_length, Player_AnimationId3_offsets);
    Timer3_Addr = FindPointerAddr(processHandle, player_base_add, Player_Timer3_offsets_length, Player_Timer3_offsets);
}