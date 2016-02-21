#ifndef IntializeFANN_H
#define IntializeFANN_H

void SetupTraining();

void GetTrainingData();

void trainFromFile(unsigned int max_neurons, const char* training_file, const char* testing_file, const char* output_file);

#endif
