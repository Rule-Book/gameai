#include <iostream>
#include <string>
#include <thread>
#include <chrono>

void printDummyLine() { // without this the last row of characters doesn't show
	std::cout << std::endl; // dummy line whose output does not display
	std::cout << "\033[A"; // move cursor above dummy line
}       

void initialPrint(char inputRows[][8]) { // use vector or list for dynamic size
	for (int i=0; i<8/*sizeof(inputRows)*/; i++) {
		for (int j=0; j<sizeof(inputRows[i]); j++) {
			std::cout << inputRows[i][j];
		}
		std::cout << std::endl;
		printDummyLine();
	}
}

void redraw(int x, int y, int len, char inputRows[][8]) {
	for (int frame = 0; frame < 8; ++frame) {
		inputRows[x][y] = 'x';

		std::cout << "\033[" << len << "A" << std::flush; // move up 8 lines

		for (int j=0; j<len; ++j) {
			std::cout << "\033[2K" << std::flush; // clear line text
			for (int i=0; i<len; ++i) {
				std::cout << inputRows[i][j];
			}
			std::cout << "\033[B"; // move cursor 1 line down
			std::cout << '\r'; // carriage return moves fully left
		}

		inputRows[x][y] = '.';
		printDummyLine();
	}
}

void movePos(int* xPtr, int* yPtr, int xWidth, int yHeight) {
	srand(time(0));
	for (int i=0; i<10; i++) {
		if (rand() % 4 == 0 && *yPtr > 0) { // north
			*yPtr -= 1; return; }
		if (rand() % 4 == 1 && *xPtr < xWidth-1) { // east
			*xPtr += 1; return; }
		if (rand() % 4 == 2 && *yPtr < yHeight-1) { // south
			*yPtr += 1; return; }
		if (rand() % 4 == 3 && *xPtr > 0) { // west
			*xPtr -= 1; return; }
	}
}

int main() {
	int n = 8;
	char rows[8][8] = {
		{'.', '.', '.', '.', '.', '.', '.', '.'},
		{'.', '.', '.', '.', '.', '.', '.', '.'},
		{'.', '.', '.', '.', '.', '.', '.', '.'},
		{'.', '.', '.', '.', '.', '.', '.', '.'},
		{'.', '.', '.', '.', '.', '.', '.', '.'},
		{'.', '.', '.', '.', '.', '.', '.', '.'},
		{'.', '.', '.', '.', '.', '.', '.', '.'},
		{'.', '.', '.', '.', '.', '.', '.', '.'}
	};
	
	initialPrint(rows);

	int xPos, yPos;
	xPos = 4; yPos = 4;
	int rowWidth=8, columnHeight=8;
	for (int i=0; i<30; i++) {
		movePos(&xPos, &yPos, rowWidth, columnHeight);
		redraw(xPos, yPos, n, rows);
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
	return 0;
}
