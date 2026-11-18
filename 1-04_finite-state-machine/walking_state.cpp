#include <string>
#include <iostream>
#include <list>
#include <thread>
#include <chrono>

class FSMState;
class FSMTransition;
class FSMStateMachine;

class FSMState
{
	public:
	virtual void onEnter() {};
	virtual void onUpdate() {};
	virtual void onExit() {};
	std::list<FSMTransition*> transitions;
};

class FSMTransition
{
	protected: // allow derived class to directly access nextState
	FSMState* nextState;
	public:
	FSMTransition(FSMState* next) : nextState(next) {}

	virtual bool isValid() { return true; };
	virtual FSMState* getNextState() { return nextState; };
	virtual void onTransition() {};
};
int xPos=4, yPos=4;
int rowWidth=8, columnHeight=8;
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

void redraw(int x, int y, int len, char inputRows[][8], char c) {
	for (int frame = 0; frame < 8; ++frame) {
		inputRows[x][y] = c;

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

class WalkState : public FSMState {
	public:
			
		void onEnter() override {
			movePos(&xPos, &yPos, rowWidth, columnHeight);
			redraw(xPos, yPos, n, rows, '1');
		};
		void onUpdate() override {
			movePos(&xPos, &yPos, rowWidth, columnHeight);
			redraw(xPos, yPos, n, rows, '2');
		};
		void onExit() override {
		};
};

class StunState : public FSMState {
	public: 
		void onEnter() override {
			redraw(xPos, yPos, n, rows, 'o');
			std::cout << std::flush;
		};
		void onUpdate() override {
			redraw(xPos, yPos, n, rows, '0');
			std::cout << "\033[2K" << std::flush; // clear line text
			std::cout << "Stuck in Stun\r" << std::flush;
	 		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		};
		void onExit() override {
		};
};

class ToStunTransition : public FSMTransition {
	public:
	ToStunTransition(FSMState* targetState) : FSMTransition(targetState) {} // set to previousState from FSMState::build
	bool isValid() override { 
		return rand() % 3 > 0; // chars[x][y]=='o';
	};
	FSMState* getNextState() override {
		return nextState;
	};
	void onTransition() override {
		std::cout << "\033[2K" << std::flush; // clear line text
		std::cout << "Enter Stun\r";
	};
};

class FromStunTransition : public FSMTransition {
	public:
	FromStunTransition(FSMState* targetState) : FSMTransition(targetState) {}
	bool isValid() override { 
		// return true; // after being stunned for 3s
		return rand() % 2 == 1; // chars[x][y]=='o';
	};
	FSMState* getNextState() override {
		return nextState;
	};
	void onTransition() override {
		std::cout << "\033[2K" << std::flush; // clear line text
		std::cout << "Exit Stun\r";
	};
};

class FiniteStateMachine
{
	public:
	void update() {
		for (std::list<FSMTransition*>::iterator it = activeState->transitions.begin(); it != activeState->transitions.end(); ++it) {
			FSMTransition* t = *it;
			if (t->isValid()) {
				t->onTransition();
				activeState->onExit();
				previousState = activeState;
				activeState = t->getNextState();
				activeState->onEnter();
				return;
			}
		}
		activeState->onUpdate();
	};

	std::list<FSMState*> states;
	FSMState* initialState;
	FSMState* activeState;
	FSMState* previousState;
	
	void build() {
		// build phase 1 create States
		FSMState* M = new WalkState();
		FSMState* S = new StunState();

		states.push_back(M);
		states.push_back(S);
		
		initialState = previousState = activeState =M;

		// build phase 2 create Transitions
		FSMTransition* s = new ToStunTransition(S);
		FSMTransition* m = new FromStunTransition(previousState);

		M->transitions.push_back(s);
		S->transitions.push_back(m);
		// above doesn't work since transition doesn't know previousState FSMState*
	}
};


int main() {
	srand(time(0));
	std::cout << "Hello, World" << std::endl;

	// create FSM
	FiniteStateMachine Wander;

	// initialize States and Transitions
	Wander.build();
	initialPrint(rows);

	// call Update loop
	for (int i=0; i<50; i++) {
		Wander.update();
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}
	

	return 0;
}
