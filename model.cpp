#include <iostream>
#include <cassert>
#include "evals.h"
#include <fstream>
using namespace std;
const int inputDim = 65;
const int h1Dim = 100;
const int h2Dim = 32;
const int outputDim = 1;

const int None = 0;
const int Pawn = 1;
const int Knight = 2;
const int Bishop = 3;
const int Rook = 4;
const int Queen = 5;
const int King = 6;
const int White = 16;
const int Black = 8;
float random() { return 2*(rand()/float(RAND_MAX))-1; }
float random2() { return (rand()/float(RAND_MAX)); }
float ReLU(float x){return x>0? x:0.0;} 
float dReLU(float x) {return x>float(0) ? float(1):float(0);}
void printArr(int n, float arr[])
{
	for (int i = 0; i < n; i++)
	{
		cout << arr[i] << " ";
	}
	cout << endl;
}
void printMatrix(int n, int m, float arr[][h1Dim])
{
	for (int j = 0;j<m;j++)
	{
		for (int i = 0; i < n; i++)
		{
			cout << arr[i][j] << " ";
		}
	}
	cout << endl;
}

void fenToVec(string fen, float vec[])
{
	int index = 0;
	for (int i = 0; i < fen.length(); i++)
	{
		char c = fen[i];
		if (c == '/') continue;
		if (c == ' ') 
		{
			fen[i+1] == 'w' ? vec[index] = 1 : vec[index] = 0;
			break;
		}
		if (c>=49 && c<= 56) //c in between 1 and 8 inc (represents how many empty squares in fen)
		{
			for (int j = 0; j<(c-48); j++)
			{
				vec[index] = 0; // add that many blank squares
			}
			continue;
		}
		int team;
		int piece;
		team = isupper(c) ? White : Black;
		if (team == White) c = c+32;
		switch (c)
		{
			case 'p':
			piece = Pawn;
			break;
			case 'n':
			piece = Knight;
			break;
			case 'b':
			piece = Bishop;
			break;
			case 'r':
			piece = Rook;
			break;
			case 'q':
			piece = Queen;
			break;
			case 'k':
			piece = King;
			break;
		}
		vec[index] = (float)piece+team;
	}
}
class StockBear
{
public:
	float error = 0;
	float output[outputDim];
	StockBear()
	{
		srand(time(0));
		for (int i = 0; i < h1Dim; i++)
		{
			for (int j = 0; j < inputDim; j++)
			{
				h1weights[j][i] = random();
			}
			h1biases[i] = random();
			h1neurons[i] = 0;
		}
		for (int i = 0; i < h2Dim; i++)
		{
			for (int j = 0; j < h1Dim; j++)
			{
				h2weights[j][i] = random();
			}
			h2biases[i] = random();
			h2neurons[i] = 0;
		}
		for (int i = 0; i<outputDim;i++)
		{
			for (int j = 0; j<h2Dim;j++)
			{
				outweights[j][i] = random(); 
			}
			outbiases[i] = random();
			output[i] = 0;
		}
		
	}
	void forwardProp(float input[inputDim])
	{
		StockBear::setZero();
		for (int i = 0; i < inputDim;i++)
		{
			inputLayer[i] = input[i];
		}
		for (int i = 0; i < h1Dim; i++)
		{
			for (int j = 0; j < inputDim; j++)
			{
				h1neurons[i] += input[j] * h1weights[j][i];
			}
			h1neurons[i] += h1biases[i];
			h1neurons[i] = ReLU(h1neurons[i]);
		}
		for (int i = 0; i < h2Dim; i++)
		{
			for (int j = 0; j < h1Dim; j++)
			{
				h2neurons[i] += h1neurons[j] * h2weights[j][i];
			}
			h2neurons[i] += h2biases[i];
			h2neurons[i] = ReLU(h2neurons[i]);
		}
		for (int i = 0; i < outputDim; i++)
		{
			for (int j = 0; j < h2Dim; j++)
			{
				output[i] += h2neurons[j] * outweights[j][i];
			}
			output[i] += outbiases[i]; //linear output when doing regression
		}
		
	}
	void backProp(float target[], float inputs[][inputDim], int batchSize) {
    // Accumulate gradients over the mini-batch
    float outputGradients[outputDim] = {0.0};

    for (int batchIndex = 0; batchIndex < batchSize; ++batchIndex) {
        forwardProp(inputs[batchIndex]);
		
        // Calculate output layer gradients for each sample in the mini-batch
        for (int i = 0; i < outputDim; ++i) {
            outputGradients[i] += output[i] - target[batchIndex];
        }
    }
	error = outputGradients[0]/batchSize;

    // Update output layer weights and biases
    for (int i = 0; i < h2Dim; ++i) {
        for (int j = 0; j < outputDim; ++j) {
            outweights[i][j] -= eta * (outputGradients[j] / batchSize) * h2neurons[i];
        }
        outbiases[i] -= eta * outputGradients[i] / batchSize;
    }

    // ... Similar updates for hidden layer 2 and hidden layer 1

    // Calculate hidden layer 2 gradients
    float h2Gradients[h2Dim] = {0.0};
    for (int i = 0; i < h2Dim; ++i) {
        for (int j = 0; j < outputDim; ++j) {
            h2Gradients[i] += outputGradients[j] * outweights[i][j];
        }
        h2Gradients[i] *= dReLU(h2neurons[i]);
    }

    // ... Similar updates for hidden layer 1

    // Calculate hidden layer 1 gradients
    float h1Gradients[h1Dim] = {0.0};
    for (int i = 0; i < h1Dim; ++i) {
        for (int j = 0; j < h2Dim; ++j) {
            h1Gradients[i] += h2Gradients[j] * h2weights[i][j];
        }
        h1Gradients[i] *= dReLU(h1neurons[i]);
    }

    // ... Similar updates for input layer

    // Update input layer weights and biases
    for (int i = 0; i < inputDim; ++i) {
        for (int j = 0; j < h1Dim; ++j) {
            h1weights[i][j] -= eta * (h1Gradients[j] / batchSize) * inputLayer[i];
        }
        h1biases[i] -= eta * h1Gradients[i] / batchSize;
    }
}
	float predict(float inp[inputDim])
	{
		StockBear::forwardProp(inp);
		return output[0];
	}
	
	
	int inputLayer[inputDim];
	float h1weights[inputDim][h1Dim]; //matrix representing weights between input and HL1
    float h1biases[h1Dim]; //array representing biases of HL1
    float h2weights[h1Dim][h2Dim]; //matrix representing weights between HL2 and HL1
    float h2biases[h2Dim];//array representing biases ofHL1
    float outweights[h2Dim][outputDim];//matrix representing weights between HL2 and output
    float outbiases[outputDim]; //array representing biases between input and HL1
    float h1neurons[h1Dim]; //array representing HL1 activations
    float h2neurons[h2Dim]; //array representing HL 2 activations
	float trainingExs = 0;
	float eta = .005;
	//float alpha =  .2;

	void setZero() 
	{
		for (int i = 0; i < h1Dim; i++)
		{
			h1neurons[i] = 0;
		}
		for (int i = 0; i < h2Dim; i++)
		{
			h2neurons[i] = 0;
		}
		for (int i = 0; i<outputDim;i++)
		{
			output[i] = 0;
		}
	}
	
	void saveModelText(string filename) {
    ofstream file(filename);

    for (int i = 0; i < inputDim; ++i) {
        for (int j = 0; j < h1Dim; ++j) {
            file << to_string(h1weights[i][j]) << ",";
        }
        file << "\n";
    }
	for (int i = 0; i < h1Dim; ++i) {file << to_string(h1biases[i]) << ",";}
	file << "\n\n";
	for (int i = 0; i < h1Dim; ++i) {
        for (int j = 0; j < h2Dim; ++j) {
            file << to_string(h2weights[i][j]) << ",";
        }
        file << "\n";
    }
	for (int i = 0; i < h2Dim; ++i) {file << to_string(h2biases[i]) << ",";}
	file << "\n\n";
	
    for (int i = 0; i < h2Dim; ++i) {
        for (int j = 0; j < outputDim; ++j) {
            file << to_string(outweights[i][j]);
        }
        file << "\n";
    }
	for (int i = 0; i < outputDim; ++i) {file << to_string(outbiases[i]) << ",";}
	file << "\n\n";
    file.close();
}
};
int DATAINDEX = 0;
void loadIO(int bs, float inp[][inputDim], float out[])
{
	for (int i = 0;i<bs;i++)
	{
		string fen = DATA[DATAINDEX*2];
		fenToVec(fen, inp[i]);
		out[i] = stof(DATA[DATAINDEX*2 + 1]);
		//cout << out[i] << " " << DATA[dataIndex*2] << endl;
		DATAINDEX++;
	}
}
int main()
{
	StockBear stockbear;
	int EPOCHS = 100;
	const int batchSize = 15;
	float input[batchSize][inputDim];
	float target[batchSize];
	float XOR[12] = {float(0),float(0),float(0),float(0),float(1),float(1),float(1),float(0),float(1),float(1),float(1),float(0)};
	//cout << stockbear.predict(input) << endl;
	float errorE = 10;
		for (int i = 0; i<EPOCHS; i++)
		{	
			loadIO(batchSize, input, target);
			//cout<< input[0] <<input[1]<<target[0]<<endl;
			//stockbear.forwardProp(input);
			stockbear.backProp(target, input, batchSize);
			cout << "Epoch " << i+1 << "'s error: " << stockbear.error << endl;
			//cout<<"epoch " << i<< " ";
		}
	//cout << endl << stockbear.error << endl;
	stockbear.saveModelText("stockbear.h");//parse with python
	return 0;
}
