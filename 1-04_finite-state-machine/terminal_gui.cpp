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

	int x, y;
	x = 7; y = 0;
	for (int i=0; i<n; i++) {
		redraw(x, y, n, rows); 
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		x--;
		y++;
	}
	return 0;
}
