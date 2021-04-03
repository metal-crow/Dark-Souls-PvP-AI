#include "Source.h"
#include <stdlib.h>
#include "InitalizeFANN.h"
#include "Settings.h"

//visual state. used for auto red signing
static const int Player_visual_offsets[] = { 0x28, 0x0, 0x30, 0xC, 0x70 };
static const int Player_visual_offsets_length = 5;
static ullong visualStatus_address;
static int visualStatus;
//current selected item
#define SelectedItemBaseAddr 0xF7F8F4
static const int Player_selectedItem_offsets[] = { 0x67C, 0xC, 0x18, 0x730, 0x2D4 };
static const int Player_selectedItem_offsets_length = 5;
static ullong selectedItem_address;
static int selectedItem;

#define RedSoapstone 101

void BlackCrystalOut(){
    ResetVJoyController();
    //switch to black crystal
    iReport.bHats = ddown;
    UpdateVJD(iInterface, (PVOID)&iReport);
    Sleep(100);
    iReport.bHats = dcenter;
    UpdateVJD(iInterface, (PVOID)&iReport);
    Sleep(1000); //gotta wait for menu to change
    //use
    iReport.lButtons = square;
    UpdateVJD(iInterface, (PVOID)&iReport);
    Sleep(100);
    //yes i want to leave
    iReport.lButtons = 0x0;
    UpdateVJD(iInterface, (PVOID)&iReport);
    Sleep(100);
    iReport.lButtons = cross;
    UpdateVJD(iInterface, (PVOID)&iReport);
    Sleep(500);
    //down d pad again to go back to red sign
    iReport.bHats = ddown;
    iReport.lButtons = 0x0;
    UpdateVJD(iInterface, (PVOID)&iReport);
    Sleep(100);
    //wait
    iReport.bHats = dcenter;
    UpdateVJD(iInterface, (PVOID)&iReport);
    Sleep(10000);//10 sec is how long it takes to black crystal
}

static bool RedSignDown = false;

void PutDownRedSign(){
    ResetVJoyController();
	//press x (in case we have a message appearing), down to goto next item, and check if we selected RSS
	for (int i = 0; i < 5; i++)
	{
		iReport.bHats = cross;
		UpdateVJD(iInterface, (PVOID)&iReport);
		Sleep(100);
		ResetVJoyController();
	}

#if 0
    while (selectedItem != RedSoapstone)
	{
		ResetVJoyController();
		UpdateVJD(iInterface, (PVOID)&iReport);
		Sleep(100);
        iReport.bHats = ddown;
        UpdateVJD(iInterface, (PVOID)&iReport);
        Sleep(100);
        iReport.bHats = dcenter;
        UpdateVJD(iInterface, (PVOID)&iReport);
        Sleep(1000); //gotta wait for menu to change
        selectedItem_address = FindPointerAddr(processHandle, memorybase + SelectedItemBaseAddr, Player_selectedItem_offsets_length, Player_selectedItem_offsets);
        ReadProcessMemory(processHandle, (LPCVOID)(selectedItem_address), &(selectedItem), 4, 0);
        guiPrint(LocationHandler",2:Selected Item:%d", selectedItem);
    }
#endif
	//use RSS
    iReport.lButtons = square;
    UpdateVJD(iInterface, (PVOID)&iReport);
    Sleep(100);

	ResetVJoyController();
	UpdateVJD(iInterface, (PVOID)&iReport);

#if 0
    //change back selected item for when summoned
    iReport.bHats = ddown;
    UpdateVJD(iInterface, (PVOID)&iReport);
    Sleep(100);
    iReport.bHats = dcenter;
    UpdateVJD(iInterface, (PVOID)&iReport);
#endif

    RedSignDown = true;
}

static bool RereadPointerEndAddress = true;
static long LastRedSignTime = 0;

int main(void){
    #if FeedNeuralNet
	trainFromFile(70, NeuralNetFolderLocation"/attack_training_data.train",				 NeuralNetFolderLocation"/attack_training_data.test",   NeuralNetFolderLocation"/Attack_dark_souls_ai.net");
	trainFromFile(30, NeuralNetFolderLocation"Neural Nets/backstab_training_data.train", NeuralNetFolderLocation"/backstab_training_data.test", NeuralNetFolderLocation"/Defense_dark_souls_ai.net");
    Sleep(7000);
    #endif
    int Setuperror = SetupandLoad();
    #if !DisableAi
    if (Setuperror){
        return EXIT_FAILURE;
    }
    #endif
    #if TrainNeuralNet
        SetupTraining();
    #endif

    while (1){
    #if AutoRedSign
        guiPrint(LocationHandler",0:RereadPointerEndAddress %d", RereadPointerEndAddress);
        guiPrint(LocationHandler",1:Enemy.loc_x %f\nvisualStatus %d", Enemy.loc_x, visualStatus);
        guiPrint(LocationHandler",2:");

        if (RereadPointerEndAddress){
            ReadPointerEndAddresses(processHandle);
            visualStatus_address = FindPointerAddr(processHandle, player_base_add, Player_visual_offsets_length, Player_visual_offsets);
            selectedItem_address = FindPointerAddr(processHandle, memorybase + SelectedItemBaseAddr, Player_selectedItem_offsets_length, Player_selectedItem_offsets);
            ReadPlayer(&Enemy, processHandle, LocationMemoryEnemy);
            ReadPlayer(&Player, processHandle, LocationMemoryPlayer);
            ResetVJoyController();//just in case
            UpdateVJD(iInterface, (PVOID)&iReport);
        }
        ReadProcessMemory(processHandle, (LPCVOID)(visualStatus_address), &(visualStatus), 4, 0);//this memory read isnt directly AI related
        ReadProcessMemory(processHandle, (LPCVOID)(selectedItem_address), &(selectedItem), 4, 0);

        //if AI is a red phantom
        if (visualStatus == 2){
            RedSignDown = false;
            //check that we got the enemy's struct address by ensuring their x loc is pos.
            if (Enemy.loc_x > 0){
                RereadPointerEndAddress = false;
            }
            //once one character dies
            if (Player.hp <= 0 || Enemy.hp <= 0){
                RereadPointerEndAddress = true;
            }
            else{
                //enemy player is fairly close
                if (distance(&Player, &Enemy) < 50){
                #if TrainNeuralNet
                    GetTrainingData();
                #else
                    MainLogicLoop();
                #endif
                }
                //last resort error catching
                else{
                    RereadPointerEndAddress = true;
                }
                //if enemy player far away, black crystal out
                /*else if (!RereadPointerEndAddress){
                    guiPrint(LocationHandler",2:BlackCrystalOut");
                    RereadPointerEndAddress = true;
                    BlackCrystalOut();
                }*/
            }
        }
        //if AI in host world, and red sign not down, put down red sign
        else if (visualStatus == 0){
            //ocasionally reput down red sign(failed to join session error catcher)
            if (!RedSignDown){
                guiPrint(LocationHandler",2:PutDownRedSign");
                Sleep(10000);//ensure we're out of loading screen
                PutDownRedSign();
            } else if (clock() >= LastRedSignTime + 360000){//6 min
                LastRedSignTime = clock();
                RedSignDown = false;
            }
        }
        else{
            RereadPointerEndAddress = true;
        }
    #else
        #if TrainNeuralNet
            GetTrainingData();
        #else
            MainLogicLoop();
        #endif
    #endif
    }

    Exit();
}