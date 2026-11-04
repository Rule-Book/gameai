#include <string>
#include <iostream>

class FSMState;
class FSMTransition;
class FSMStateMachine;

class FSMState
{
	virtual void onEnter();
	virtual void onUpdate();
	virtual void onExit();
	list<FSMTransition> transitions;
};

class FSMTransition
{
	virtual bool isValid();
	virtual FSMState* getNextState();
	virtual void onTransition();
};

class FiniteStateMachine
{
	void update();
	list<FSMState> states;
	FSMState* initialState;
	FSMState* activeState;
};

int main() {
	std::cout << "Hello, World" << std::endl;
	return 0;
}
