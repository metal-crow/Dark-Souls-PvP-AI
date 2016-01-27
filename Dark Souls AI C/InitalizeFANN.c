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

#define PI 3.14159265

volatile bool listening = true;
volatile bool inTraining = false;

DWORD WINAPI ListentoContinue(void* data) {
    printf("e to exit, t to train, p to pause\n");
    while (listening){
        char input = getchar();
        if (input=='e'){//exit
            listening = false;
        } 
        else if (input == 't'){//train
            inTraining = true;
        } 
        else if (input == 'p'){//pause
            inTraining = false;
        }
    }
    return 0;
}

#define RRAND(min,max) (min + rand() % (max - min))
#define backstab_animation 225

/*
void getTrainingDataforBackstab(void)
{
    FILE* fpdef = fopen("E:/Code Workspace/Dark Souls AI C/Neural Nets/backstab_training_data.train", "a");

    unsigned int trainingLinesCountDef = 0;

    //memset to ensure we dont have unusual char attributes at starting
    memset(&Enemy, 0, sizeof(Character));
    memset(&Player, 0, sizeof(Character));

    HANDLE processHandle = readingSetup();

    CharState** stateBuffer = calloc(4, sizeof(CharState*));
    //initalize so we dont read null
    stateBuffer[1] = ReadPlayerFANN(&Enemy, processHandle);
    stateBuffer[0] = ReadPlayerFANN(&Player, processHandle);

    HANDLE thread = CreateThread(NULL, 0, ListentoContinue, NULL, 0, NULL);

    while (listening){
        //listen to both chars, use during normal play
        //read the state of the player and enemy every couple seconds, and keep a buffer of their states.
        //when we find the player is in a specific state(i.e backstab if we're training defense), get the last state of the enemy and use it here
        //also, ocasionally get an enemy state when player not in state, and use it

        //mathmaticly impossible to not double count AND never miss with random ping times
        //also cant check if the last buffer enemy was in backstab, because they could immediatly do another baskstab, and we'll miss a new one
        //this works pretty well, never misses, rairly double counts
        Sleep(RRAND(2500, 4000));

        readPointers(processHandle);

        //move buffer down
        if (stateBuffer[3] && stateBuffer[4]){
            free(stateBuffer[3]);
            free(stateBuffer[2]);
        }

        stateBuffer[3] = stateBuffer[1];
        stateBuffer[2] = stateBuffer[0];

        stateBuffer[1] = ReadPlayerFANN(&Enemy, processHandle);
        stateBuffer[0] = ReadPlayerFANN(&Player, processHandle);

        //trigger on backstab
        bool bsActivateState = stateBuffer[1]->animation_id == backstab_animation;

        //check state and train
        if (
            (bsActivateState || (rand() < 800))
            && inTraining
           )
        {
            float distance = distanceFANN(stateBuffer[2], stateBuffer[3]);
            float angleDelta = angleDeltaFromFrontFANN(stateBuffer[2], stateBuffer[3]);
            float rotationDelta = rotationDifferenceFromSelfFANN(stateBuffer[2], stateBuffer[3]);//rotation with respect to self rotation

            //write the input floats, then the output float
            fprintf(fpdef, "%f %f %f %f %f\n", 
                distance,
                angleDelta,
                stateBuffer[3]->velocity,
                rotationDelta,
                (bsActivateState ? 1.0 : 0.0)
                );
            trainingLinesCountDef++;

            //save
            printf("backstab:%s distance %f, angleD %f, velocity %f, rotation enemy %f\n", bsActivateState ? "true" : "false", distance, angleDelta, stateBuffer[3]->velocity, rotationDelta);
        }
    }

    fprintf(fpdef, "## = %d\n", trainingLinesCountDef);

    fclose(fpdef);
}
*/

//have random attacks. if it doesnt get hit, sucess. if it gets hit, fail.
void getTrainingDataforAttack(void)
{
    FILE* fpatk = fopen("E:/Code Workspace/Dark Souls AI C/Neural Nets/attack_training_data.train", "a");

    unsigned int trainingLinesCountAtk = 0;

    SetupandLoad();

    HANDLE thread = CreateThread(NULL, 0, ListentoContinue, NULL, 0, NULL);

    while (listening){
        MainLogicLoop();

        if (subroutine_states[AttackTypeIndex] == GhostHitId){
            int startingHp = Player.hp;

            while (isAttackAnimation(Player.animationType_id)){
                ReadPlayer(&Player, processHandle, LocationMemoryPlayer);
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
            trainingLinesCountAtk++;

            //output the array of distance values
            for (int i = 0; i < DistanceMemoryLENGTH; i++){
                fprintf(fpatk, "%f ", DistanceMemory[i]);
            }
            //output estimated stamina
            fprintf(fpatk, "%f ", (float)StaminaEstimationEnemy());
            //TODO output the enemy's current poise
            //TODO output the attack's poise damage
            //TODO output the AI's current poise
            //TODO output array of AI's HP over time
            //TODO output array of AI's subanimations
            //output result
            fprintf(fpatk, "%f\n", result);

            printf("result:%f\n", result);
        }
    }

    fprintf(fpatk, "%d 51 1\n", trainingLinesCountAtk);

    fclose(fpatk);
}


//use the file to train the network
int trainFromFile(void){
    //create new, empty network
    struct fann *net = fann_create_shortcut(2, 47, 1);//set up net without hidden layers. N inputs, 1 output
    fann_set_training_algorithm(net, FANN_TRAIN_RPROP);
    fann_set_activation_function_hidden(net, FANN_SIGMOID_SYMMETRIC);
    fann_set_activation_function_output(net, FANN_LINEAR);
    fann_set_train_error_function(net, FANN_ERRORFUNC_LINEAR);
    fann_set_bit_fail_limit(net, (fann_type)0.9);
    fann_set_train_stop_function(net, FANN_STOPFUNC_BIT);
    fann_print_parameters(net);

    //load training data
    unsigned int max_neurons = 30;
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

int mainFANN(void){
    getTrainingDataforAttack();
    //trainFromFile();
    return 0;
}