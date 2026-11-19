#include <iostream>
#include <cstdint>

void printComputerRNG(int seed) {
	std::cout << seed << std::endl;
	std::srand(seed);
	for (int i=0; i<4; i++) {
		for (int j=0; j<4; j++)
			std::cout << rand() % 2;
		if (i < 3)
		       std::cout << " | ";
	}
	std::cout << std::endl;
}

void printFilteredRNG(int seed) {
	std::cout << seed << std::endl;
	std::srand(seed);
	for (int i=0; i<4; i++) {
		for (int j=0; j<4; j++) {
			bool n = rand() % 2;
			std::cout << n;
		}
		if (i < 3)
		       std::cout << " | ";
	}
	std::cout << std::endl;
}

int main() {
	// int seed = time(0);
	int seed = 123456;
	std::cout << "RNG Seed: " << seed << std::endl;
	// uint8_t a = 2;
	printComputerRNG(seed);	
	printFilteredRNG(seed);	
	return 0;
}
