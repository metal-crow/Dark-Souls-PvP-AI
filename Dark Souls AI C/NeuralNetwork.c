#include "NeuralNetwork.h"
#include <stdlib.h> 
#include <stdbool.h>
#include <math.h>

Neuron* InitalizeNeuron(unsigned int numInputs){
	Neuron* neuron = malloc(sizeof(Neuron));
	neuron->numInputs = numInputs+1;
	neuron->weights = calloc(numInputs+1, sizeof(double));

	//start neuron with randomized weights(+1 for bias)
	for (unsigned int i = 0; i < numInputs+1; i++){
		neuron->weights[i] = (double)rand();
	}

	return neuron;
}
//helper function for factorial
static unsigned int factorial(const unsigned int a){
	unsigned int aFactorial = 1;
	for (unsigned int i = 1; i <= a; i++){
		aFactorial *= i;
	}
	return aFactorial;
}

//helper function for binomial coefficent(n choose k)
static unsigned int choose(const unsigned int n, const unsigned int k){
	return (factorial(n) / (factorial(k)*factorial(n - k)));
}

//we want to calculate the combinations from the set of inputs for numInputs set size
//i.e from inputs a,b,c,d we need a,b a,c a,d etc, and give each set to a neuron
static unsigned int* generateInputMapping(unsigned int numberOfInputs, unsigned int combinationSize, unsigned int outputSetSize){
	unsigned int* mappings = malloc(sizeof(unsigned int)*combinationSize*outputSetSize);//store mappings linearly as 1d and parse as 2d later

	//set default starting set (e.x 0,1 for setsize 2)
	unsigned int* currentState = malloc(sizeof(unsigned int)*outputSetSize);
	for (unsigned int i = 0; i < outputSetSize; i++){
		currentState[i] = i;
	}

	for (unsigned int i = 0; i < combinationSize*outputSetSize; i += outputSetSize){
		//store current mapping
		for (unsigned int j = 0; j < outputSetSize; j++){
			mappings[i + j] = currentState[j];
		}
		//increment state to next combination
		for (unsigned int j = outputSetSize - 1; j >= 0; j--){
			//when this sigfig reaches max
			if (currentState[j] == numberOfInputs - 1){
				currentState[j] = currentState[j - 1] + 2;
			} else{
				//only increment this current sig, exit
				currentState[j]++;
				break;
			}
		}
	}
	free(currentState);
	return mappings;
}

#if 0
#include <stdio.h>
int main(void){
	unsigned int* map = generateInputMapping(3, choose(3, 2), 2);
	for (unsigned int i = 0; i < choose(3, 2) * 2; i += 2){
		for (unsigned int j = 0; j < 2; j++){
			printf("%d,", map[i + j]);
		}
		printf("\n");
	}
}
#endif

NeuronLayer* InitalizeNeuronLayer(unsigned int numInputs, unsigned int NumInputsPerNeuron, bool CombinationsinputMappingType){
	unsigned int numNeurons;
	if (CombinationsinputMappingType){
		numNeurons = choose(numInputs, NumInputsPerNeuron);//calculate number of neurons via binomial coefficient
	} else{
		numNeurons = numInputs / NumInputsPerNeuron;
	}

	NeuronLayer* layer = malloc(sizeof(NeuronLayer));
	layer->numNeurons = numNeurons;
	layer->neurons = calloc(numNeurons, sizeof(Neuron));
	//calculate input to neuron input mappings
	if (CombinationsinputMappingType){
		//combinations mappings
		layer->inputMappings = generateInputMapping(numInputs, numNeurons, NumInputsPerNeuron);
	} else{
		//basic linear mappings
		layer->inputMappings = malloc(sizeof(unsigned int)*numInputs);
		for (unsigned int i = 0; i<numInputs; i++){
			layer->inputMappings[i] = i;
		}
	}

	//add neurons
	for (unsigned int i = 0; i < numNeurons; i++){
		layer->neurons[i] = InitalizeNeuron(NumInputsPerNeuron);
	}

	return layer;
}

NeuralNet* InitalizeNeuronNet(unsigned int numInputs){
	unsigned int numLayers = 3;

	NeuralNet* net = malloc(sizeof(NeuralNet));
	net->numInputs = numInputs;
	net->numLayers = numLayers;
	net->neuronLayers = calloc(numLayers, sizeof(NeuronLayer));

	//add neuron layers
	net->neuronLayers[0] = InitalizeNeuronLayer(numInputs, 2, true);//this layer is a BINARY combination of all inputs
	net->neuronLayers[1] = InitalizeNeuronLayer(choose(numInputs, 2), 2, false);//this layer is a filtering step. Narrows outputs, limits work each neuron has to do, increase number of ajustable weights.
	net->neuronLayers[2] = InitalizeNeuronLayer(choose(numInputs, 2) / 2, choose(numInputs, 2) / 2, false);//final summation of layers outputs(1 neuron)

	return net;
}

static double Sigmoid(double input){
    return 1.0 / (1.0 + exp(-input));
}

double* Process(NeuralNet* net, double* input){
	double* output=NULL;
	for (unsigned int layeri = 0; layeri < net->numLayers; layeri++){
        NeuronLayer* layer = net->neuronLayers[layeri];
		//use output from last layer as input
		if (layeri > 0){
            if (layeri > 1){//dont free the original input, only the input which was output
                free(input);
            }
			input = output;
		}
        output = calloc(layer->numNeurons, sizeof(double));

		unsigned int currentMap = 0;
		for (unsigned int i = 0; i < layer->numNeurons; i++){
			double netinput = 0;
			//for each neuron sum the (corrisponding inputs * corresponding weights).Throw
			//the total at our sigmoid function to get the output.
			for (unsigned int j = 0; j < layer->neurons[i]->numInputs - 1; j++){
				netinput += layer->neurons[i]->weights[j] * input[layer->inputMappings[currentMap]];
				currentMap++;
			}

			//add in bias weight
			netinput += layer->neurons[i]->weights[layer->neurons[i]->numInputs] * -1;

			//we can store the outputs from each layer as we generate them.
			//The combined activation is first filtered through the sigmoid
			//function
			output[i] = Sigmoid(netinput);
		}
	}
	return output;
}

void train(NeuralNet* net, double* input, double desiredOutput){
    //here we calculate the input and output values along the neurons in each layer
    //from input to layer 0 to layer 0 output
    double* Layer0In = calloc(net->neuronLayers[0]->numNeurons, sizeof(double));
    double* Layer0Out = calloc(net->neuronLayers[0]->numNeurons, sizeof(double));
    for (unsigned int neuronnum = 0; neuronnum < net->neuronLayers[0]->numNeurons; neuronnum++){//for every neuron in this layer
        Neuron* neuron = net->neuronLayers[0]->neurons[neuronnum];

        Layer0In[neuronnum] = neuron->weights[neuron->numInputs-1];  /* start with the bias */
        for (unsigned int i = 0; i < neuron->numInputs - 1; i++) { /* i loop over layer 1 weights */
            Layer0In[neuronnum] += input[net->neuronLayers[0]->inputMappings[neuronnum + i]] * neuron->weights[i];     /* add in weighted contributions from prev layer (using input map) */
        }
        Layer0Out[neuronnum] = Sigmoid(Layer0In[neuronnum]); /* compute sigmoid to give activation */
    }

    //from layer 0 output to layer 1 to layer 1 output
    double* Layer1In = calloc(net->neuronLayers[1]->numNeurons, sizeof(double));
    double* Layer1Out = calloc(net->neuronLayers[1]->numNeurons, sizeof(double));
    for (unsigned int neuronnum = 0; neuronnum < net->neuronLayers[1]->numNeurons; neuronnum++){
        Neuron* neuron = net->neuronLayers[1]->neurons[neuronnum];

        Layer1In[neuronnum] = neuron->weights[neuron->numInputs-1];
        for (unsigned int i = 0; i < neuron->numInputs - 1; i++) {
            Layer1In[neuronnum] += Layer0Out[net->neuronLayers[1]->inputMappings[neuronnum+i]] * neuron->weights[i];
        }
        Layer1Out[neuronnum] = Sigmoid(Layer1In[neuronnum]);
    }

    //from layer 1 output to layer 2 to layer 2 output
    double* Layer2In = calloc(net->neuronLayers[2]->numNeurons, sizeof(double));
    double* Layer2Out = calloc(net->neuronLayers[2]->numNeurons, sizeof(double));
    for (unsigned int neuronnum = 0; neuronnum < net->neuronLayers[2]->numNeurons; neuronnum++){
        Neuron* neuron = net->neuronLayers[2]->neurons[neuronnum];

        Layer2In[neuronnum] = neuron->weights[neuron->numInputs-1];
        for (unsigned int i = 0; i < neuron->numInputs - 1; i++) {
            Layer2In[neuronnum] += Layer1Out[net->neuronLayers[2]->inputMappings[neuronnum + i]] * neuron->weights[i];
        }
        Layer2Out[neuronnum] = Sigmoid(Layer2In[neuronnum]);
    }

    //compute error for layer 2(only 1 neuron)
    double error = desiredOutput - Layer2Out[0];
    //find new weight based on each input
    Neuron* neuronL2 = net->neuronLayers[2]->neurons[0];
    neuronL2->weights[neuronL2->numInputs] += error;//ajust bias
    //ajust weight
    for (unsigned int i = 0; i < neuronL2->numInputs - 1; i++){
        neuronL2->weights[i] += error* Layer1Out[net->neuronLayers[2]->inputMappings[i]];//NEW WEIGHT = WEIGHT + ERROR * INPUT
    }

    //compute error for layer 1
    for (unsigned int i = 0; i < net->neuronLayers[1]->numNeurons; i++){
        Neuron* neuron = net->neuronLayers[1]->neurons[i];
        double error = neuron->weights[neuron->numInputs] - Layer1Out[i];//desired outbut is bias threshold of l2 - output of this neuron
        neuron->weights[neuron->numInputs] += error;//ajust bias
        //backpropagate to layer 1
        for (unsigned int j = 0; j < neuron->numInputs - 1; j++){
            neuron->weights[j] += error * Layer0Out[net->neuronLayers[1]->inputMappings[i + j]];
        }
    }

    //compute error for layer 0
    for (unsigned int i = 0; i < net->neuronLayers[0]->numNeurons; i++){
        Neuron* neuron = net->neuronLayers[0]->neurons[i];
        double error = neuron->weights[neuron->numInputs] - Layer0Out[i];
        neuron->weights[neuron->numInputs] += error;//ajust bias
        //backpropagate to layer 0
        for (unsigned int j = 0; j < neuron->numInputs - 1; j++){
            neuron->weights[j] += error * input[net->neuronLayers[0]->inputMappings[i + j]];
        }
    }
}

#if 0
#include <stdio.h>

static void printWeights(NeuralNet* net){
    for (unsigned int i = 0; i < net->numLayers; i++){
        printf("Layer %d:\n", i);
        for (unsigned int j = 0; j < net->neuronLayers[i]->numNeurons; j++){
            printf("\tN %d[", j);
            for (unsigned int k = 0; k < net->neuronLayers[i]->neurons[j]->numInputs; k++){
                printf("%.3f,", net->neuronLayers[i]->neurons[j]->weights[k]);
            }
            printf("]\n");
        }
        printf("\n");
    }
}

int main(void){
    NeuralNet* net = InitalizeNeuronNet(4);
    double* input = malloc(sizeof(double) * 4);
    input[0] = 1000.0;
    input[1] = 50.0;
    input[2] = 500.0;
    input[3] = 2.0;
    double desiredOutput = 0;

    double* result = Process(net, input);
    printf("before training: %f\n", result[0]);
    free(result);

    double* resultAfter;
    do{
        printWeights(net);
        train(net, input, desiredOutput);

        resultAfter = Process(net, input);
        printf("training: %f\n", resultAfter[0]);
    } while (resultAfter[0] != desiredOutput);

    resultAfter = Process(net, input);
    printf("after training final: %f\n", resultAfter[0]);
    free(resultAfter);

    free(input);

    return EXIT_SUCCESS;
}
#endif