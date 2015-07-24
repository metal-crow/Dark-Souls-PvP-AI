//program to train, teach, and create neural net
#pragma warning(disable: 4244)
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <math.h>
#include <stdbool.h>

#include "MemoryEdits.h"
#include "CharacterStruct.h"

#include "fann.h"

typedef struct {
    float loc_x;
    float loc_y;
    unsigned int r_weapon_id;
    float rotation;
    unsigned char animation_id;
    float velocity;
}CharState;

Character Enemy;
Character Player;

void readPointers(HANDLE processHandle){
    Enemy.location_x_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_loc_x_offsets_length, Enemy_loc_x_offsets);
    Enemy.location_y_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_loc_y_offsets_length, Enemy_loc_y_offsets);
    Enemy.r_weapon_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_r_weapon_offsets_length, Enemy_r_weapon_offsets);
    Enemy.rotation_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_rotation_offsets_length, Enemy_rotation_offsets);
    Enemy.animation_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_animation_offsets_length, Enemy_animation_offsets);
    Enemy.velocity_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_velocity_offsets_length, Enemy_velocity_offsets);

    Player.location_x_address = FindPointerAddr(processHandle, player_base_add, Player_loc_x_offsets_length, Player_loc_x_offsets);
    Player.location_y_address = FindPointerAddr(processHandle, player_base_add, Player_loc_y_offsets_length, Player_loc_y_offsets);
    Player.r_weapon_address = 0;
    Player.rotation_address = FindPointerAddr(processHandle, player_base_add, Player_rotation_offsets_length, Player_rotation_offsets);
    Player.animation_address = 0;
    Player.velocity_address = 0;
}

HANDLE readingSetup(){
    //get access to dark souls memory
    char * processName = "DARKSOULS.exe";
    //get the process id from the name
    int processId = GetProcessIdFromName(processName);
    //open the handle
    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, processId);
    //get the base address of the process and append all other addresses onto it
    ullong memorybase = GetModuleBase(processId, processName);
    Enemy_base_add += memorybase;
    player_base_add += memorybase;

    readPointers(processHandle);

    return processHandle;
}

#define PI 3.14159265

CharState* ReadPlayerFANN(Character * c, HANDLE processHandle){
    CharState* state = malloc(sizeof(CharState));

    //read x location
    ReadProcessMemory(processHandle, (LPCVOID)(c->location_x_address), &(state->loc_x), 4, 0);
    //read y location
    ReadProcessMemory(processHandle, (LPCVOID)(c->location_y_address), &(state->loc_y), 4, 0);
    //read r hand wep
    if (c->r_weapon_address){
        ReadProcessMemory(processHandle, (LPCVOID)(c->r_weapon_address), &(state->r_weapon_id), 4, 0);
    }

    ReadProcessMemory(processHandle, (LPCVOID)(c->rotation_address), &(state->rotation), 4, 0);
    //Player rotation is pi. 0 to pi,-pi to 0. Same as atan2
    //convert to radians, then to degrees
    c->rotation = (c->rotation + PI) * (180.0 / PI);

    //read current animation id
    if (c->animation_address){
        ReadProcessMemory(processHandle, (LPCVOID)(c->animation_address), &(state->animation_id), 2, 0);
    }

    if (c->velocity_address){
        ReadProcessMemory(processHandle, (LPCVOID)(c->velocity_address), &(state->velocity), 4, 0);
    }

    return state;
}

//get straight line distance between me and enemy
float distanceFANN(CharState * Player, CharState * Phantom){
    double delta_x = fabsf(fabsf(Player->loc_x) - fabsf(Phantom->loc_x));
    double delta_y = fabsf(fabsf(Player->loc_y) - fabsf(Phantom->loc_y));
    return hypot(delta_x, delta_y);
}

//the absolute value of the angle the opponent is off from straight ahead
float angleDeltaFromFrontFANN(CharState * Player, CharState * Phantom){
    double delta_x = fabsf(fabsf(Player->loc_x) - fabsf(Phantom->loc_x));
    double delta_y = fabsf(fabsf(Player->loc_y) - fabsf(Phantom->loc_y));

    //if its closer to either 90 or 270 by 45, its x direction facing
    if (((Player->rotation > 45) && (Player->rotation < 135)) || ((Player->rotation > 225) && (Player->rotation < 315))){
        return atan(delta_y / delta_x);
    } else{
        return atan(delta_x / delta_y);
    }
}

float rotationDifferenceFromSelfFANN(CharState * Player, CharState * Phantom){
    double delta = fabs((Player->rotation) - (Phantom->rotation));
    return delta;
}

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

int num_input = 4;
int num_output = 1;

#define newFile
//TODO decide on inputs
//listen and save data to train on to file
int getTrainingData(void)
{
    FILE* fp;
    //if file doesnt exist
#ifdef newFile
        printf("new training file\n");
        fp = fopen("E:/Code Workspace/Dark Souls AI C/Neural Nets/backstab_training_data.train", "w");
        //first line is number of training sets, number or inputs, and number of outputs
        fprintf(fp, "## %d %d\n", num_input, num_output);
#else
    //else append new training data to it
        printf("Appending to training file\n");
        fp = fopen("E:/Code Workspace/Dark Souls AI C/Neural Nets/backstab_training_data.train", "a");
#endif

    unsigned int trainingLinesCount = 0;

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
        Sleep(RRAND(2500, 5000));

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

        bool backstabState = stateBuffer[1]->animation_id == backstab_animation;

        //check state and train
        if (
            ( backstabState || (rand() < 1000) )
            && inTraining
           )
        {
            float distance = distanceFANN(stateBuffer[2], stateBuffer[3]);
            float angleDelta = angleDeltaFromFrontFANN(stateBuffer[2], stateBuffer[3]);
            float rotationDelta = rotationDifferenceFromSelfFANN(stateBuffer[2], stateBuffer[3]);//rotation with respect to self rotation

            //write the input floats, then the output float
            fprintf(fp, "%f %f %f %f %f\n", 
                distance,
                angleDelta,
                stateBuffer[3]->velocity,
                rotationDelta,
                (backstabState ? 1.0 : 0.0)
                );
            trainingLinesCount++;

            //save
            printf("backstab:%s distance %f, angleD %f, velocity %f, rotation enemy %f\n", backstabState ? "true" : "false", distance, angleDelta, stateBuffer[3]->velocity, rotationDelta);
        }
    }

    fprintf("## = %d\n", trainingLinesCount);

    fclose(fp);

    return 0;
}

//use the file to train the network
int trainFromFile(void){
    //create new, empty network
    struct fann *net = fann_create_shortcut(2, num_input, num_output);//set up net without hidden layers. 4 inputs, 1 output
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

    struct fann_train_data *data = fann_read_train_from_file("E:/Code Workspace/Dark Souls AI C/Neural Nets/backstab_training_data.train");

    //train network
    fann_cascadetrain_on_data(net, data, max_neurons, 0, desired_error);

    //TODO test trained network on test data

    //save and clean up
    fann_print_connections(net);

    fann_save(net, "E:/Code Workspace/Dark Souls AI C/Neural Nets/backstab_train.net");

    fann_destroy_train(data);
    fann_destroy(net);

    return 0;
}

int main(void){
    getTrainingData();
}