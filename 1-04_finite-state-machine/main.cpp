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
	public:
	virtual bool isValid() { return true; };
	virtual FSMState* getNextState() { return nullptr; };
	virtual void onTransition() {};
};

class FiniteStateMachine
{
	public:
	void update() {};
	std::list<FSMState> states;
	FSMState* initialState;
	FSMState* activeState;
};

class NakedState : public FSMState {
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

class ShirtState : public FSMState {
	public: 
		void onEnter() override {
			std::cout << "Entered Shirt FSMState" << std::endl;
		};
		void onUpdate() override {
			std::cout << "Update Shirt FSMState" << std::endl;
		};
		void onExit() override {
			std::cout << "Exit Shirt FSMState" << std::endl;
		};
};

int main() {
	std::cout << "Hello, World" << std::endl;
	Naked suitUp;
	suitUp.onEnter();
	suitUp.onUpdate();
	suitUp.onExit();
	FiniteStateMachine OutfitStateMachine;
	for (int i=0; i<10; i++) {
		OutfitStateMachine.update();
	}
	return 0;
}
