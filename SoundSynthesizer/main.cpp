#include <iostream>
using namespace std;

#include "olcNoiseMaker.h"

double w(double dHertz)
{
	return dHertz * 2.0 * PI;
}


// Converts Frequency (Hz) to angular velocity


double osc(double dHertz, double dTime, int nType)
{

	switch (nType)
	{
	case 0: // sine wave
		return sin(w(dHertz)* dTime);
	case 1: // square wave
		return sin(w(dHertz)* dTime) > 0.0 ? 1.0 : -1.0;
	case 2: // triangle wave
		return asin(sin(w(dHertz)* dTime)) * 2.0/PI;
	case 3: // sawtooth wave (analogue, warm slow)
	{
		double dOutput = 0.0;
		for (double n = 1.0; n < 10.0; n++)
			dOutput += (sin(n*w(dHertz)*dTime)) / n;
		return dOutput * (2.0 / PI);
	}
	case 4: // saw wave (optimized harsh fast)
		return (2.0 / PI)*(dHertz*PI*fmod(dTime, 1.0 / dHertz) - (PI / 2.0));
	case 5: //pseudo random noise
		return 2.0* ((double)rand() / (double)RAND_MAX) - 1.0;
	default: 
		return 0.0;
	}
}

struct sEnvelopeADSR
{
	double dAttackTime;
	double dDecayTime;
	double dReleaseTime;
	double dSustainAmplitude;
	double dStartAmplitude;

	double dTriggerOnTime;
	double dTriggerOffTime;

	bool bNoteOn;

	sEnvelopeADSR()
	{
		dAttackTime = 0.10;
		dDecayTime = 0.05;
		dStartAmplitude = 1.0;
		dSustainAmplitude = .80;
		dReleaseTime = 0.20;
		dTriggerOffTime = 0.0;
		dTriggerOffTime = 0.0;
		bNoteOn = false;
	}

	double GetAmplitude(double dTime)
	{
		double dAmplitude = 0.0;
		double dLifeTime = dTime - dTriggerOnTime;

		if (bNoteOn)
		{
			// ADS

			// Attack
			if (dLifeTime <= dAttackTime)
				dAmplitude = (dLifeTime / dAttackTime)* dStartAmplitude;

			// Decay
			if (dLifeTime > dAttackTime && dLifeTime <= (dAttackTime + dDecayTime))
				dAmplitude = ((dLifeTime - dAttackTime) / dDecayTime)*(dSustainAmplitude - dStartAmplitude) + dStartAmplitude;
			// sustain
			if (dLifeTime > (dAttackTime + dDecayTime))
				dAmplitude = dSustainAmplitude;
		}
		else
		{
			// Release
			dAmplitude = ((dTime - dTriggerOffTime) / dReleaseTime)*(0.0 - dSustainAmplitude)+ dSustainAmplitude;
		}

		if (dAmplitude <= 0.0001)
		{
			dAmplitude = 0;
		}

		return dAmplitude;
	};

	// call when key is pressed
	void NoteOn(double dTimeOn)
	{
		dTriggerOnTime = dTimeOn;
		bNoteOn = true;
	}

	void NoteOff(double dTimeOff)
	{
		dTriggerOffTime = dTimeOff;
		bNoteOn = false;
	}


};

atomic<double> dFrequencyOutput = 0.0;
sEnvelopeADSR envelope;
double dOctaveBaseFrequency = 110.0; // A2  Frequency of octave represented by Hertz. Can toggle to higher value. 
double d12thRootOf2 = pow(2.0, 1.0 / 12.0); // assuming western 12 notes/octave


double MakeNoise(double dTime)
{
	double dOutput = envelope.GetAmplitude(dTime) * 
		(
			+ osc(dFrequencyOutput  *0.5, dTime, 3)
			+ osc(dFrequencyOutput * 1.5, dTime, 1)

		);  // Toggle 0,1,2,3,4

	
	return dOutput *.5; // Master Volume
}

int main()
{
    wcout << "Yiiiiii Haw. Let's play some Piano" << endl;

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

	int nCurrentKey = -1;
	bool bKeyPressed = false;
	while (1)
	{
		bKeyPressed = false;
		/*if (GetAsyncKeyState((unsigned char)("Q")) & 0x8000)
	dOctaveBaseFrequency = 220.0;*/ // my failed code to raise the octave
		for (int k = 0; k < 16; k++)
		{

				if (GetAsyncKeyState((unsigned char)("ZSXCFVGBNJMK\xbcL\xbe\xbf"[k])) & 0x8000)
				{					

					if (nCurrentKey != k)
					{
					dFrequencyOutput = dOctaveBaseFrequency * pow(d12thRootOf2, k);
					envelope.NoteOn(sound.GetTime());
					wcout << "\rNote On : " << sound.GetTime() << "s " << dFrequencyOutput << "Hz";
					nCurrentKey = k;
					}
				bKeyPressed = true;
				}
		}

		if (!bKeyPressed)
		{
			if (nCurrentKey != -1)
			{
				wcout << "\rNote Off: " << sound.GetTime() << "s                             ";
				envelope.NoteOff(sound.GetTime());
				nCurrentKey = -1;
			}
		}
	}
	return 0;
}