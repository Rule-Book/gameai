#include <string>
#include <iostream>
#include <list>

class FSMState;
class FSMTransition;
class FSMStateMachine;

class FSMState
{
	virtual void onEnter();
	virtual void onUpdate();
	virtual void onExit();
	std::list<FSMTransition> transitions;
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
	std::list<FSMState> states;
	FSMState* initialState;
	FSMState* activeState;
};

int main() {
	std::cout << "Hello, World" << std::endl;
	return 0;
}
