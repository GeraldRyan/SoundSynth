#include <iostream>
using namespace std;

#include "olcNoiseMaker.h"


atomic<double> dFrequencyOutput = 0.0;

double MakeNoise(double dTime)
{

	double dOutput = 1.0*sin(dFrequencyOutput * 2 * 3.14159 *dTime); 

	// return dOutput *.5; // sine wave toggle

	// square wave toggle
	if (dOutput > 0.0)
		return 0.921;
	else
		return -0.921;

	}



int main()
{
    wcout << "Yiiiiii Haw Buddy. Let's play some Piano" << endl;

	// Get all sound HW
	vector<wstring> devices = olcNoiseMaker<short>::Enumerate();

	// Display Findings
	for (auto d : devices) wcout << "Found Output Device\n";

	// Display a keyboard
	wcout << endl <<
		"|   |   |   |   |   | |   |   |   |   | |   | |   |   |   |" << endl <<
		"|   | S |   |   | F | | G |   |   | J | | K | | L |   |   |" << endl <<
		"|   |___|   |   |___| |___|   |   |___| |___| |___|   |   |__" << endl <<
		"|     |     |     |     |     |     |     |     |     |     |" << endl <<
		"|  Z  |  X  |  C  |  V  |  B  |  N  |  M  |  ,  |  .  |  /  |" << endl <<
		"|_____|_____|_____|_____|_____|_____|_____|_____|_____|_____|" << endl << endl;

	// Create Sound Machine
	olcNoiseMaker<short> sound(devices[0], 44100,1,8,512); // can change shorts to char for 8 bit or int for 32 bits- for audiophiles only. 

	// link noise function with sound machine
	sound.SetUserFunction(MakeNoise);

	double dOctaveBaseFrequency = 110.0; // A2
	double d12thRootOf2 = pow(2.0, 1.0 / 12.0);
	

	while (1)
	{
		// add a keyboard like a piano

		bool bKeyPressed = false;
		for (int k = 0; k < 16; k++)
		{
			if (GetAsyncKeyState((unsigned char)("ZSXCFVGBNJMK\xbcL\xbe\xbf"[k])) & 0x8000)
			{
				dFrequencyOutput = dOctaveBaseFrequency * pow(d12thRootOf2, k);
				bKeyPressed = true;

			}
		}

		if (!bKeyPressed)
		{
			dFrequencyOutput = 0.0;
		}



	}









	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
