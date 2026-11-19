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
		result += interpolateSigmoidAcrossPoints(t, x1, y1, x2, y2, 0.8, i);
		// there are numRanges or 2n-1+1
		// 2 at 1
		// 3 at 2
		// 5 at 3
		// 9 at 4
		yValCopy.erase(yValCopy.begin(), yValCopy.begin() + numRanges); // lazy pop value instead of managing dynamic list of arrays
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

void colorSet(double normalizedColor) {
	// int color = (int)(normalizedColor * 256);
	// std::cout << "\033[38;5;" << (normalizedColor%256) << "m";
	std::cout << normalizedColor;
	int colorLevel = (int)(normalizedColor * 5.999);
	int idx = 16 + 36*colorLevel + 6*colorLevel + colorLevel;
	//std::cout << idx;
	std::cout << "\033[38;5;" << idx << "m";
	// there doesn't seem to be an online resource that's linearly interpolated across 256 ansi colors
	// I would have to create my own interpretation of the ansi colors available and how to interpolate between them
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
	int t; // f(t) for sigmoid fn
	int p; // persistence, drives amplitude of octaves
	uint8_t i = 3; // octave
	std::vector<float> ybounds;
	/* hardcoded values from gameaipro
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
	*/
	srand(time(0));
	for (int n=0; n<expectedValsForOctave(i); n++)
		ybounds.push_back((float)((rand() >> 8) % 256)/256);
	if ((ybounds.empty()) || ybounds.size() != expectedValsForOctave(i)) {
		std::cout << "Expected yVals.size(): " << (int)expectedValsForOctave(i) << '/' << ybounds.size() << std::endl;
		return 0;
	}
	// there should be 2^(n-1) + 1 equidistant ranges of sigmoid functions per octave
	// octave 1 has one range [0,5),[5,1]
	// octave 2 has three ranges [0,.3),[.3,.6),[.6,1]
	// octave 3 has five ranges...
	// sigmoid function interpolates between arbitrary random y values along the x ranges
	// each octave is multiplied by persistence^(octave) and summed
		   // amplitude of octave = pow(p,i)
	double output;
	int color = 0;
	std::cout << "Output: " << std::endl;
	initialPrint(rows);
	for (double d=0.0; d<=1.0; d+=0.01) {
		output = sigmoid(d, i, ybounds);
		// Change color based on result, print, then return color back to white
		std::this_thread::sleep_for(std::chrono::milliseconds(60));
		redraw((int)(d*20), (int)(output*(-8)+8), 8, rows, '@', output); 
	}
	return 0;
}
