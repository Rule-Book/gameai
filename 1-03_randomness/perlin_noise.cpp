#include <iostream>
#include <cmath>

double sigmoid(double abc) {
	double result = 0.0;
	result = 6*pow(abc,5) - 15*pow(abc,4) + 10*pow(abc,3); // sigmoid (smooth) function
	return result;
}
int main() {
	int t; // f(t) for sigmoid fn
	int p; // persistence, drives amplitude of octaves
	// there should be 2^(n-1) + 1 equidistant ranges of sigmoid functions per octave
	// octave 1 has one range [0,5),[5,1]
	// octave 2 has three ranges [0,.3),[.3,.6),[.6,1]
	// octave 3 has five ranges...
	// sigmoid function interpolates between arbitrary random y values along the x ranges
	// each octave is multiplied by persistence^(octave) and summed
	int i = 1; // octave
		   // amplitude of octave = pow(p,i)
	return 0;
}
