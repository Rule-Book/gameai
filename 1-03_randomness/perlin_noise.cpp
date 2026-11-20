#include <iostream>
#include <cmath>
#include <cstdint>
#include <vector>
#include <thread>
#include <chrono>
#include <string>

uint8_t expectedValsForOctave(uint8_t i) {
	// there should be 2^(n-1) + 1 equidistant ranges of sigmoid functions per octave
	// octave 1 has two numbers [0,5),[5,1]
	// octave 2 has three numbers [0,.3),[.3,.6),[.6,1]
	// octave 3 has five numbers... below line returns summation of all octaves
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

double sigmoid(double t, uint8_t numOctaves, std::vector<float> yVals, float p) {
	// calculate expected number of yVals before starting
	if ((yVals.empty()) || yVals.size() != expectedValsForOctave(numOctaves))
		return -1;
	std::vector<float> yValCopy;
	yValCopy.assign(yVals.begin(), yVals.end()); // copy w/o affecting original
	double result = 0.0;

	for (uint8_t i = 1; i <= numOctaves; i++) {
		int values = pow(2, i-1) + 1; //rename to numbers, and numRanges-1 to numRanges
		// do I need to calculate every octave before I can return anything?
		// yes but only for the specific t value
		// at most, one sigmoid sum per octave, and stretching sigmoid to match range
		double rangeWidth = 1.0/(values-1); // 2 numbers at octave 1, but only 1 range
		int octaveHorizontalSegment = (int)(t/rangeWidth);
		double x1 = octaveHorizontalSegment * rangeWidth;
		double x2 = (octaveHorizontalSegment+1) * rangeWidth;
		double y1 = yValCopy.at(octaveHorizontalSegment);
		double y2 = yValCopy.at(octaveHorizontalSegment+1);
		result += interpolateSigmoidAcrossPoints(t, x1, y1, x2, y2, p, i);
		// remove values from summed octave
		yValCopy.erase(yValCopy.begin(), yValCopy.begin() + values);
	}
	result = result / 1.533; // reduce amplitude to compensate increasing persistence from 0.5-0.8 (for more waves)
	return result;
}

char rows[8][20] = {
	{'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
	{'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
	{'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
	{'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
	{'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
	{'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
	{'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
	{'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'}
};

void printDummyLine(double output) {
	std::cout << output  << std::endl;
	std::cout << std::endl;
	std::cout << "\033[A";
	std::cout << "\033[A";
}

void initialPrint(char inputRows[][20]) {
	for (int i=0; i<8; i++) {
		for (int j=0; j<sizeof(inputRows[i]); j++) {
			std::cout << inputRows[i][j];
		}
		std::cout << std::endl;
		printDummyLine(0.0);
	}
}

void cubeColorSet(double t) {
	int color = (int)(t * 5);
	int r, g, b;
	if (t < 0.25) {
		double s = t/0.25;
		r=0; g=(int)(s*255); b=255;
	} else if (t < 0.5) {
		double s = (t-0.25) / 0.25;
		r=0; g=255; b=(int)(1-s) * 255;
	} else if (t < 0.75) {
		double s = (t-0.5) / 0.25;
		r=(int)(s*255); g=255; b=0;
	} else {
		double s = (t-0.75) / 0.25;
		r=255; g=(int)((1-s)*255); b=0;
	}
	std::cout << "\033[38;2;" 
		<< r << ";"
		<< g << ";"
		<< b << "m";
}

void colorUnSet() {
	std::cout << "\033[0m";
}

void redraw(int x, int y, /*int wid,*/ int hei, char inputRows[][20], char c, double clr) {
	inputRows[y][x] = c; // 2d array first index decides height...
	int wid = sizeof(inputRows[0]);
	for (int frame = 0; frame < hei; ++frame) {
		std::cout << "\033[" << hei << "A" << std::flush; // move up 8 lines

		for (int i=0; i<hei; ++i) {
			std::cout << "\033[2K" << std::flush; // clear line text
			for (int j=0; j<wid; ++j) {
				if (j == x && i == y) {
					cubeColorSet(clr);
					std::cout << c;
					colorUnSet();
					continue;
				}
				std::cout << inputRows[i][j];
			}
			std::cout << "\033[B"; // move cursor 1 line down
			std::cout << '\r'; // carriage return moves fully left
		}

		printDummyLine(clr);
	}
	//inputRows[x][y] = '.';
}

int main() {
	// perlin noise interpolates between arbitrary random y values along each octave's x ranges

	int t; // f(t) for sigmoid fn
	float p = 0.8; // persistence, drives amplitude of octaves
	uint8_t i = 3; // octave
	std::vector<float> ybounds;
	srand(time(0));
	for (int n=0; n<expectedValsForOctave(i); n++)
		ybounds.push_back((float)((rand() >> 8) % 256)/256);
	if ((ybounds.empty()) || ybounds.size() != expectedValsForOctave(i)) {
		std::cout << "Expected yVals.size(): " << (int)expectedValsForOctave(i) << '/' << ybounds.size() << std::endl;
		return 0;
	}
	double output;
	int color = 0;
	std::cout << "Output: " << std::endl;
	initialPrint(rows);

	for (double d=0.0; d<=1.0; d+=0.01) {
		output = sigmoid(d, i, ybounds, p);
		if (output < -1.0 || 1.5 < output) {
			std::cout << "Output: " << output << " out of bounds!" << std::endl;
			return -1;
		}
		// Change color based on result, print, then return color back to white
		std::this_thread::sleep_for(std::chrono::milliseconds(60));
		redraw((int)(d*20), (int)(output*(-8)+8), 8, rows, '@', output);
	}

	return 0;
}
