#include <string>
#include <iostream>
#include <list>

class FSMState;
class FSMTransition;
class FSMStateMachine;

class Unit;

class GameState
{
	public:
	std::list<Unit*> characters;
	std::list<FSMStateMachine*> characterFSMs;
	GameState() {};
	/// std::list<Waypoint> coordinates;
	bool calculateLOS(bool placeholder) { return placeholder; };
};

class Unit
{
	protected:
	int damage = 1;
	int health = 10;
	int atkRange = 1;
	int atkSpeed = 2;
	// int moveSpeed;
	int cooldown = 1;
	/// int attackType = attack.MELEE;
	public:
	Unit(int dmg, int hp, int atkRng, int atkSpd, int cd) {
		damage = dmg;
		health = hp;
		atkRange = atkRng;
		atkSpeed = atkSpd;
		cooldown = cd;
	}
	Unit* target;
	// Waypoint* destination;
	/// virtual void getAttackType() { return attackType };
};

class FSMState
{
	public:
	GameState* gameState;
	Unit* unit;
	FSMState(GameState* gs, Unit* u) {
		gameState = gs;
		unit = u;
	}
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

class IdentifyTarget : public FSMState {
	public:
	IdentifyTarget(GameState* gs, Unit* u) : FSMState(gs, u) {}
		void onEnter() override {
			std::cout << "Entered IdentifyTarget FSMState" << std::endl;
		};
		void onUpdate() override {
			std::cout << "Update IdentifyTarget FSMState" << std::endl;
		};
		void onExit() override {
			std::cout << "Exit IdentifyTarget FSMState" << std::endl;
		};
};

class ApproachTarget : public FSMState {
	public: 
	ApproachTarget(GameState* gs, Unit* u) : FSMState(gs, u) {}
		void onEnter() override {
			std::cout << "Entered ApproachTarget FSMState" << std::endl;
		};
		void onUpdate() override {
			std::cout << "Update ApproachTarget FSMState" << std::endl;
		};
		void onExit() override {
			std::cout << "Exit ApproachTarget FSMState" << std::endl;
		};
};

class LostTarget : public FSMTransition {
	public:
	LostTarget(FSMState* targetState) : FSMTransition(targetState) {}
	bool isValid() override { 
		return true;
	};
	FSMState* getNextState() override {
		return nextState;
	};
	void onTransition() override {
		std::cout << "reIdentifyTarget after LostTarget" << std::endl;
	};
};

class FoundTarget : public FSMTransition {
	public:
	FoundTarget(FSMState* targetState) : FSMTransition(targetState) {}
	bool isValid() override { 
		return true;
	};
	FSMState* getNextState() override {
		return nextState;
	};
	void onTransition() override {
		std::cout << "ApproachTarget after FoundTarget" << std::endl;
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
		FSMState* identify = new IdentifyTarget(nullptr, nullptr);
		FSMState* approach = new ApproachTarget(nullptr, nullptr);

		states.push_back(identify);
		states.push_back(approach);
		
		initialState = activeState = identify;

		// build phase 2 create Transitions
		FSMTransition* reidentify = new LostTarget(identify);
		FSMTransition* reapproach = new FoundTarget(approach);

		identify->transitions.push_back(reapproach);
		approach->transitions.push_back(reidentify);
	}
};

int main() {
	std::cout << "Hello, World" << std::endl;
	FiniteStateMachine character1;
	character1.build();
	for (int i=0; i<10; i++) {
		character1.update();
	}
	return 0;
}
