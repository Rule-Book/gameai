#include <iostream>
#include <cmath>
#include <cstdint>
#include <vector>
#include <thread>
#include <chrono>
#include <string>

uint8_t expectedValsForOctave(uint8_t i) {
	return (1 << i) + i - 1;
}

double interpolateSigmoidAcrossPoints(double t, double x1, double y1, double x2, double y2, double p, uint8_t i) {
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

double sigmoid(double t, uint8_t numOctaves, std::vector<float> yVals) {
	// calculate expected number of yVals before starting
	if ((yVals.empty()) || yVals.size() != expectedValsForOctave(numOctaves))
		return -1;
	std::vector<float> yValCopy;
	yValCopy.assign(yVals.begin(), yVals.end()); // copy w/o affecting original
	double result = 0.0;

	for (uint8_t i = 1; i <= numOctaves; i++) {
	int numRanges = pow(2, i-1) + 1; //rename to numbers, and numRanges-1 to numRanges
	// do I need to calculate every octave before I can return anything?
	// yes but only for the specific t value
	// at most, one sigmoid sum per octave, and stretching sigmoid to match range
	double rangeWidth = 1.0/(numRanges-1); // 2 numbers at octave 1, but only 1 range
	int octaveHorizontalSegment = (int)(t/rangeWidth);
	double x1 = octaveHorizontalSegment * rangeWidth; // drop remainder to get clean x-ranges
	double x2 = (octaveHorizontalSegment+1) * rangeWidth;
	double y1 = yValCopy.at(octaveHorizontalSegment);
	double y2 = yValCopy.at(octaveHorizontalSegment+1);
		result += interpolateSigmoidAcrossPoints(t, x1, y1, x2, y2, 0.5, i);
		// there are numRanges or 2n-1+1
		// 2 at 1
		// 3 at 2
		// 5 at 3
		// 9 at 4
		yValCopy.erase(yValCopy.begin(), yValCopy.begin() + numRanges); // lazy pop value instead of managing dynamic list of arrays
	}
	return result;
}

int main() {
	int t; // f(t) for sigmoid fn
	int p; // persistence, drives amplitude of octaves
	uint8_t i = 3; // octave
	std::vector<float> ybounds;
	ybounds.push_back(0.01); // Octave 1
	ybounds.push_back(0.99);
	ybounds.push_back(0.38); // Octave 2
	ybounds.push_back(0.88);
	ybounds.push_back(0.0);
	ybounds.push_back(0.2); // Octave 3
	ybounds.push_back(0.015);
	ybounds.push_back(0.5);
	ybounds.push_back(0.935);
	ybounds.push_back(0.6);
	// there should be 2^(n-1) + 1 equidistant ranges of sigmoid functions per octave
	// octave 1 has one range [0,5),[5,1]
	// octave 2 has three ranges [0,.3),[.3,.6),[.6,1]
	// octave 3 has five ranges...
	// sigmoid function interpolates between arbitrary random y values along the x ranges
	// each octave is multiplied by persistence^(octave) and summed
		   // amplitude of octave = pow(p,i)
	double output;
	std::cout << "Output: " << std::endl;
	int color = 255;
	std::string blocks(40, '@');
	for (double d=0.0; d<=1.0; d+=0.01) {
		output = sigmoid(d, i, ybounds);
		color = (int)(output * 256);
		if (color < 0)
			continue;
		int bNum = (int)(d * 40);
		// Change color based on result, print, then return color back to white
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		std::cout << "\033[A\r\033[K";
		std::cout << "\033[38;5;" << (color%256) << "m" << blocks.substr(0, bNum) << "\033[38;5;255m" << std::endl;
	}
	if (color < 0)
		std::cout << "Expected yVals.size() : " << (int)expectedValsForOctave(i) << '/' << ybounds.size() << std::endl;
	return 0;
}
