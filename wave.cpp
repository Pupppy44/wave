/*
* This program creates a pure tone wave file.
* Waveforms are pure tone because they only possess one constant frequency.
* Futher, if we transform a sine wave to a frequency domain using a Fourier transform, we get only one, distinct value.
*/
#include <windows.h> // For PlaySound
#include <iostream>
#include <fstream>
#include <map>
#include <string>
using namespace std;

#define SAMPLE_RATE 44100 // Amount of samples of the sound taken per second. Determines the maximum frequency of the audio.
#define BIT_DEPTH   16 // Resolution of each sample of the audio. Determines the maximum amplitude of the audio.
#define DURATION    2 // The duration of the audio in seconds.
#define PI          3.141592653589793 // Mathematical constant. Not edible. Required for the formula.

std::map<int, std::string> notes = { {0, "C"}, {1, "C#"}, {2, "D"}, {3, "D#"}, {4, "E"}, {5, "F"},
									{6, "F#"}, {7, "G"}, {8, "G#"}, {9, "A"}, {10, "A#"}, {11, "B"} };

std::string convertToNote(double frequency) {
	// Calculate the note number based on frequency
	double noteNum = 12 * log2(frequency / 440) + 69;
	int noteIndex = (int)round(noteNum) % 12;

	// Get the octave number
	int octave = (int)round(noteNum) / 12 - 1;

	// Look up the note name from the list
	std::string note = notes[noteIndex];

	// Return the note name with the octave
	return note + std::to_string(octave);
}

// The sine occilator class, also known as the core of this program.
class sine {
public:
	/*
	* The constructor requires a frequency and amplitude. The phase is set to 0.
	* The frequency is the amount of cycles per second measured in Hertz. The amplitude is the strength, or volume, of the waveform.
	* As stated in the Nyquist-Shannon sampling theorem, these values are the minimum required to create a pure tone.
	*/
	sine(float _f, float _a) : freq(_f), amp(_a), offset(2 * PI * freq / SAMPLE_RATE) {};

	inline float process() {
		/*
		 * Formula: A * sin(2 * PI * f * t)
		* aka: A * sin(2 * PI * f / sr)
		 */
		auto sample = amp * sin(angle);
		angle += offset;
		return sample;
	}

	// Saves the raw audio data as a .wav file to the current directory. Default name is "sine.wav."
	void save() {
		ofstream file;
		file.open("sine.wav", ios::binary);

		// Lambda function to simplify writing the header and data.
		auto write = [&file](int value, int size) {
			file.write((char*)(&value), size);
		};

		// Write the header. Necessary for the audio to be playable as a .wav file.
		file << "RIFF";
		file << "----";
		file << "WAVE";
		file << "fmt ";

		write(16, 4);
		write(1, 2);
		write(1, 2);
		write(SAMPLE_RATE, 4);
		write(SAMPLE_RATE * BIT_DEPTH / 8, 4);
		write(BIT_DEPTH / 8, 2);
		write(BIT_DEPTH, 2);

		file << "data";
		file << "----";

		int pre_audio_position = file.tellp();

		auto max_amplitude = pow(2, BIT_DEPTH - 1) - 1;

		for (int i = 0; i < SAMPLE_RATE * DURATION; i++) {
			write((short)(process() * max_amplitude), 2);
		}

		int post_audio_position = file.tellp();

		file.seekp(pre_audio_position - 4);
		write(post_audio_position - pre_audio_position, 4);

		file.seekp(4, ios::beg);
		write(post_audio_position - 8, 4);

		file.close();
	}

	float freq, amp, angle = 0.f, offset;
};

int main() {
	float frequency = 0.f;
	float amplitude = 0.f;

	cout << "Enter a frequency: ";
	if (!(cin >> frequency)) return system("echo Invalid input, exiting & exit");
	cout << "Enter an amplitude: ";
	if (!(cin >> amplitude)) return system("echo Invalid input, exiting & exit");

	cout << "Harmonic Note: " << convertToNote(frequency) << endl;

	sine sine(frequency, amplitude);
	sine.save();

	PlaySoundA("sine.wav", NULL, SND_FILENAME | SND_SYNC);

	return 0;
}