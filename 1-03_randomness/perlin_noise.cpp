#include <iostream>
#include <cmath>
#include <cstdint>
#include <list>
#include <thread>
#include <chrono>

double interpolateSigmoidAcrossPoints(double t, double x1, double y1, double x2, double y2, double p, int i) {
	if (t < x1 || x2 < t) { // confirm input lies within x-boundaries
		std::cout << "Out of Bounds [" << x1 << " < " << t << " < " << x2 << "]" << std::endl;
		return 0;
	}
	double result = 0.0;
	t = t - x1; // shift right to 1st x-boundary
	t = t / (x2-x1); // squash horizontally to 2nd x-boundary
	
	result = 6*pow(t,5) - 15*pow(t,4) + 10*pow(t,3); // sigmoid (smooth) function
	
	result = result * (y2-y1); // squash vertically towards 2nd y-boundary
	result = result + y1; // shift up to 1st y-boundary
	result = result * pow(p, i); // multiply by amplitude
	return result;
}	       

double sigmoid(double t, uint8_t octaves, std::list<float> yVals) {
	int numRanges = pow(2, octaves-1) + 1;
	// do I need to calculate every octave before I can return anything?
	// yes but only for the specific t value
	// at most, one sigmoid sum per octave, and stretching sigmoid to match range
	double rangeWidth = 1.0/numRanges;
	double x1 = (int)(t/rangeWidth) * rangeWidth; // drop remainder to get clean x-ranges
	double x2 = x1 + rangeWidth;

	double result = 0.0;
	for (uint8_t i = 1; i <= octaves; i++) {
		result += interpolateSigmoidAcrossPoints(t, x1, 0.0, x2, 1.0, 0.5, i);
	}
	return result;
}

int main() {
	int t; // f(t) for sigmoid fn
	int p; // persistence, drives amplitude of octaves
	int i = 1; // octave
	// there should be 2^(n-1) + 1 equidistant ranges of sigmoid functions per octave
	// octave 1 has one range [0,5),[5,1]
	// octave 2 has three ranges [0,.3),[.3,.6),[.6,1]
	// octave 3 has five ranges...
	// sigmoid function interpolates between arbitrary random y values along the x ranges
	// each octave is multiplied by persistence^(octave) and summed
		   // amplitude of octave = pow(p,i)
	double output;
	std::cout << "Output: " << std::endl;
	std::list<float> ybounds;
	ybounds.push_back(0.9);
	ybounds.push_back(0.1);
	output = sigmoid(0.3, 1, ybounds);
	int color = (int)(output * 256);
	// Change color based on result, print, then return color back to white
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	std::cout << "\r\033[K"
	std::cout << "\033[38;5;" << (color%256) << "m" << output << "\033[38;5;255m" << std::endl;
	return 0;
}
