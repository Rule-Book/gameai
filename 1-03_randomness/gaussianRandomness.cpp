#include <iostream>
#include <cmath>
#include <cstdint>

uint32_t seed = 61829450; // holds 26/32 bits of data

double GaussianRand() {
	double sum = 0; // holds 64 bits of data

	int K = 3;
	for (int i=0; i<K; i++) {
		uint32_t holdseed = seed;
		seed = seed ^ seed << 13; // xorShift algorithm for randomness
		seed ^= seed >> 17; // 13 17 5 is the best theoretical pseudo rng for 32 bit numbers, use 13 7 17 for 64-bit
		seed ^= seed << 5;
		int32_t r= (int64_t)(holdseed + seed); // cut off any bits past 32
		sum += (double)r * (1.0/0x7FFFFFFF/*FFFFFFFF*/); // 64 bits where the first bit is 0 so it's positive
	}
	return sum; // returns [-3.0, 3.0] at (66.7%, 95.8%, 100%) stdev is sqrt(K/3)
}

int main() {
seed = 970100;
	std::cout << GaussianRand() << std::endl;
}
