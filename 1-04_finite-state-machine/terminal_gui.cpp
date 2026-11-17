#include <iostream>
#include <string>
#include <thread>
#include <chrono>

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
	for (int i=0; i<n; i++) {
		for (int j=0; j<n; j++) {
			std::cout << rows[i][j];
		}
		std::cout << std::endl;
	}

	int x, y;
	x = 0; y = 0;
	for (int frame = 0; frame < 8; ++frame) {
		rows[x][y] = 'x';

		std::cout << "\033[" << n << "A" << std::flush; // move up 8+1 lines

		for (int i=0; i<n; ++i) {
			std::cout << "\033[2K" << std::flush; // clear line text
			for (int j=0; j<n; ++j) {
				std::cout << rows[i][j];
			}
			std::cout << "\033[B"; // move cursor 1 line down
			std::cout << '\r'; // carriage return moves fully left
		}

		rows[x][y] = '.';
	std::cout << std::endl; // empty line since final line doesn't print in full
	std::cout << "\033[A";
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		x++;
		y++;
	}

	return 0;
}
