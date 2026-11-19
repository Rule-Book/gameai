#include <iostream>
#include <cstdint>
#include <bitset>

const int seed = 123456;
const int period = 10; //bitset won't work without a constant value
const int samples = 4;

void printComputerRNG(int s, int p, int sa) {
	std::srand(seed);
	for (int i=0; i<samples; i++) {
		for (int j=0; j<period; j++)
			std::cout << rand() % 2;
		if (i < samples-1)
		       std::cout << " | ";
	}
	std::cout << std::endl;
}

void printFilteredRNG(int s, int p, int sa) {
	uint16_t flipHistory = 0;
	std::srand(seed);
	for (int i=0; i<samples; i++) {
		for (int j=0; j<period; j++) {
			bool isHeads = rand() % 2;
			// if all 3 previous bits match
			if ((isHeads == (flipHistory >> 2) % 2)
				&& (isHeads == (flipHistory >> 1) % 2)
				&& (isHeads == (flipHistory) % 2)) {
				isHeads = !isHeads; // flip coin 
			}
			flipHistory = flipHistory << 1; // shift history by 1
			flipHistory += isHeads;
		}
		std::cout << std::bitset<period>(flipHistory);
		if (i < samples-1) // print <period> bits 
		       std::cout << " | ";
	}
	std::cout << std::endl;
}

int main() {
	// int seed = time(0);
	std::cout << "RNG Seed: " << seed << std::endl;
	// uint8_t a = 2;
	printComputerRNG(seed, period, samples);	
	printFilteredRNG(seed, period, samples);	
	return 0;
}
