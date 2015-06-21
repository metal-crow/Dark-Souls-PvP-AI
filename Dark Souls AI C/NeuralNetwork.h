#ifndef NeuralNetwork_H
#define NeuralNetwork_H

//Im aware im doing stuff wrong, ESPECIALLY the training backpropagation.

typedef struct {
	unsigned int numInputs;
	double * weights;
}Neuron;

typedef struct {
	unsigned int numNeurons;
	unsigned int* inputMappings;
	Neuron** neurons;
}NeuronLayer;

typedef struct {
	unsigned int numInputs;
	unsigned int numLayers;
	NeuronLayer** neuronLayers;
}NeuralNet;

NeuralNet* InitalizeNeuronNet();

double* Process(NeuralNet* net, double* input);

//todo weight ajusting for learning

#endif
