#include <string>
#include <iostream>
#include <list>

class FSMState;
class FSMTransition;
class FSMStateMachine;

class FSMState
{
	public:
	virtual void onEnter() {};
	virtual void onUpdate() {};
	virtual void onExit() {};
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

class Naked : public FSMState {
	public:
		void onEnter() override {
			std::cout << "Entered Naked FSMState" << std::endl;
		};
		void onUpdate() override {
			std::cout << "Update Naked FSMState" << std::endl;
		};
		void onExit() override {
			std::cout << "Exit Naked FSMState" << std::endl;
		};
};

int main() {
	std::cout << "Hello, World" << std::endl;
	Naked suitUp;
	suitUp.onEnter();
	suitUp.onUpdate();
	suitUp.onExit();
	return 0;
}
