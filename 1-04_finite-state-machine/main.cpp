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

class WearShirtFromNakedStateTransition : public FSMTransition {
	public:
	WearShirtFromNakedStateTransition(FSMState* targetState) : FSMTransition(targetState) {}
	bool isValid() override { 
		return true;
	};
	FSMState* getNextState() override {
		return nextState;
	};
	void onTransition() override {
		std::cout << "Wearing Shirt from Naked" << std::endl;
	};
};

class TakeOffShirtFromShirtStateTransition : public FSMTransition {
	public:
	TakeOffShirtFromShirtStateTransition(FSMState* targetState) : FSMTransition(targetState) {}
	bool isValid() override { 
		return true;
	};
	FSMState* getNextState() override {
		return nextState;
	};
	void onTransition() override {
		std::cout << "Taking off shirt from Shirt State" << std::endl;
	};
};

class FiniteStateMachine
{
	public:
	void update() {
		for (std::list<FSMTransition*>::iterator it = activeState->transitions.begin(); it != activeState->transitions.end(); ++it) {
			FSMTransition* t = *it; // dereference array of transitions to single pointer
			if (t->isValid()) {
				t->onTransition();
				activeState->onExit();
				activeState = t->getNextState();
				activeState->onEnter();
				std::cout << "=======" << std::endl;
				return;
			}
		}
		activeState->onUpdate();
	};
	std::list<FSMState*> states; /* used to store unique references
				       to all states for ptr reuse
				       across transtions */
	FSMState* initialState;
	FSMState* activeState;
	
	void build() {
		// build phase 1 create States and set starting States
		FSMState* naked = new NakedState();
		FSMState* shirt = new ShirtState();

		states.push_back(naked);
		states.push_back(shirt);
		
		initialState = activeState = naked;

		// build phase 2 create Transitions
		FSMTransition* wearShirt = new WearShirtFromNakedStateTransition(shirt);
		FSMTransition* takeOffShirt = new TakeOffShirtFromShirtStateTransition(naked);

		naked->transitions.push_back(wearShirt);
		shirt->transitions.push_back(takeOffShirt);
	}
};

int main() {
	std::cout << "Hello, World" << std::endl;
	FiniteStateMachine OutfitStateMachine;
	OutfitStateMachine.build();
	for (int i=0; i<10; i++) {
		OutfitStateMachine.update();
	}
	return 0;
}
