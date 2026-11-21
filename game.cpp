#include <string>
#include <iostream>
#include <list>

class FSMState;
class FSMTransition;
class FSMStateMachine;

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
	char icon = '_';
	public:
	Unit(int dmg, int hp, int atkRng, int atkSpd) {
		damage = dmg;
		health = hp;
		atkRange = atkRng;
		atkSpeed = atkSpd;
	}
	void reduceHealth(int hp) { health -= hp; };
	int getHealth() { return health; };
	void setIcon(char c) { icon = c; };
	char getIcon() { return icon; };
	Unit* target;
	void cooldownAttack() { cooldown = atkSpeed; };
	int getCooldown() { return cooldown; };
	void decreaseCooldown() { cooldown -= 1; };
	// Waypoint* destination;
	/// virtual void getAttackType() { return attackType };
};

class GameState
{
	public:
	std::list<Unit*> characters;
	std::list<FSMStateMachine*> characterFSMs;
	GameState() {};
	/// std::list<Waypoint> coordinates;
	bool calculateLOS(bool placeholder) { return placeholder; };
	bool targetInRange(Unit* attacker, Unit* target) { 
		// return Distance(u1, u2) < u1->atkRange;
		return true; };
	Unit* getTargetFor(Unit* character) {
		// return character;
		if (characters.front() == character)
		{
			return characters.back();
		}
		return characters.front();
	}
};

class FSMState
{
	public:
	virtual void onEnter(GameState* gs, Unit* u) {};
	virtual void onUpdate(GameState* gs, Unit* u) {};
	virtual void onExit(GameState* gs, Unit* u) {};
	std::list<FSMTransition*> transitions;
};

class FSMTransition
{
	protected: // allow derived class to directly access nextState
	FSMState* nextState;
	public:
	FSMTransition(FSMState* next) : nextState(next) {}

	virtual bool isValid(GameState* gs, Unit* u) { return true; };
	virtual FSMState* getNextState(GameState* gs, Unit* u) { return nextState; };
	virtual void onTransition(GameState* gs, Unit* u) {};
};

class IdentifyTarget : public FSMState {
	public:
			
		void onEnter(GameState* gs, Unit* u) override {
			std::cout << "Entered IdentifyTarget FSMState" << std::endl;
		};
		void onUpdate(GameState* gs, Unit* u) override {
			std::cout << "Update IdentifyTarget FSMState" << std::endl;
		};
		void onExit(GameState* gs, Unit* u) override {
			std::cout << "Exit IdentifyTarget FSMState" << std::endl;
		};
};

class ApproachTarget : public FSMState {
	public: 
		void onEnter(GameState* gs, Unit* u) override {
			std::cout << "Entered ApproachTarget FSMState" << std::endl;
		};
		void onUpdate(GameState* gs, Unit* u) override {
			std::cout << "Update ApproachTarget FSMState" << std::endl;
		};
		void onExit(GameState* gs, Unit* u) override {
			std::cout << "Exit ApproachTarget FSMState" << std::endl;
		};
};

class AttackTarget : public FSMState {
	public: 
		void onEnter(GameState* gs, Unit* u) override {
			u->cooldownAttack();
			std::cout << "Entered AttackTarget FSMState" << std::endl;
		};
		void onUpdate(GameState* gs, Unit* u) override {
			if (u->getCooldown() > 0) {
				std::cout << u->getIcon() << " cooldown remaining before attacking: ";
				std::cout << u->getCooldown() << std::endl;
				u->decreaseCooldown();
				return;
			}

			u->cooldownAttack();
			std::cout << "Update AttackTarget FSMState" << std::endl;
			int damage = 2;
			u->target->reduceHealth(damage);
			std::cout << u->getIcon() << " attacked " << u->target->getIcon();
			std::cout << " dealing " << damage << " damage!" << std::endl;
		};
		void onExit(GameState* gs, Unit* u) override {
			std::cout << "Exit AttackTarget FSMState" << std::endl;
		};
};

class Dead : public FSMState {
	public: 
		void onEnter(GameState* gs, Unit* u) override {
			std::cout << "Enter Dead State" << std::endl;
		};
		void onUpdate(GameState* gs, Unit* u) override {
			std::cout << "Dying Update" << std::endl;
		};
		void onExit(GameState* gs, Unit* u) override {
			std::cout << "Exit Dead FSMState" << std::endl;
		};
};

class LostTarget : public FSMTransition {
	public:
	LostTarget(FSMState* targetState) : FSMTransition(targetState) {}
	bool isValid(GameState* gs, Unit* u) override { 
		bool noLongerHasTarget = ( u->target == nullptr );
		return noLongerHasTarget;
	};
	FSMState* getNextState(GameState* gs, Unit* u) override {
		return nextState;
	};
	void onTransition(GameState* gs, Unit* u) override {
		std::cout << "reIdentifyTarget after LostTarget" << std::endl;
	};
};

class FoundTarget : public FSMTransition {
	public:
	FoundTarget(FSMState* targetState) : FSMTransition(targetState) {}
	bool isValid(GameState* gs, Unit* u) override { 
		bool hasLineOfSight = gs->calculateLOS( true );
		u->target = gs->getTargetFor(u);
		return hasLineOfSight;
	};
	FSMState* getNextState(GameState* gs, Unit* u) override {
		return nextState;
	};
	void onTransition(GameState* gs, Unit* u) override {
		u->target = gs->getTargetFor(u);
		std::cout << u->getIcon() << " FoundTarget " << u->target->getIcon() << std::endl;
		std::cout << "ApproachTarget after FoundTarget" << std::endl;
	};
};

class StartAttack : public FSMTransition {
	public:
	StartAttack(FSMState* targetState) : FSMTransition(targetState) {}
	bool isValid(GameState* gs, Unit* u) override { 
		bool canAttack = gs->targetInRange(u, u->target);
		return canAttack;
	};
	FSMState* getNextState(GameState* gs, Unit* u) override {
		return nextState;
	};
	void onTransition(GameState* gs, Unit* u) override {
		std::cout << u->getIcon() << " Starting to attack " << u->target->getIcon() << std::endl;
	};
};

class DyingTransition : public FSMTransition {
	public:
	DyingTransition(FSMState* targetState) : FSMTransition(targetState) {}
	bool isValid(GameState* gs, Unit* u) override { 
		return u->getHealth() < 1;
	};
	FSMState* getNextState(GameState* gs, Unit* u) override {
		return nextState;
	};
	void onTransition(GameState* gs, Unit* u) override {
		std::cout << "Dying" << std::endl;
	};
};

class FiniteStateMachine
{
	GameState* gameState;
	Unit* unit;
	public:
	FiniteStateMachine(GameState* gs, Unit* u) {
		gameState = gs;
		unit = u;
	}
	void update() {
		for (std::list<FSMTransition*>::iterator it = activeState->transitions.begin(); it != activeState->transitions.end(); ++it) {
			FSMTransition* t = *it; // dereference array of transitions to single pointer
			if (t->isValid(gameState, unit)) {
				t->onTransition(gameState, unit);
				activeState->onExit(gameState, unit);
				activeState = t->getNextState(gameState, unit);
				activeState->onEnter(gameState, unit);
				std::cout << "=======" << std::endl;
				return;
			}
		}
		activeState->onUpdate(gameState, unit);
	};
	std::list<FSMState*> states; /* used to store unique references
				       to all states for ptr reuse
				       across transtions */
	FSMState* initialState;
	FSMState* activeState;
	
	void build() {
		// build phase 1 create States and set starting States
		FSMState* identify = new IdentifyTarget();
		FSMState* approach = new ApproachTarget();
		FSMState* attack = new AttackTarget();
		FSMState* death = new Dead();

		states.push_back(identify);
		states.push_back(approach);
		states.push_back(death);
		
		initialState = activeState = identify;

		// build phase 2 create Transitions
		FSMTransition* reidentify = new LostTarget(identify);
		FSMTransition* reapproach = new FoundTarget(approach);
		FSMTransition* attacking = new StartAttack(attack);
		FSMTransition* dying = new DyingTransition(death);

		identify->transitions.push_back(dying);
		identify->transitions.push_back(reapproach);
		approach->transitions.push_back(dying);
		approach->transitions.push_back(reidentify);
		approach->transitions.push_back(attacking);
		attack->transitions.push_back(dying);
		attack->transitions.push_back(reidentify);
	}
};

int main() {
	std::cout << "Hello, World" << std::endl;
	GameState game;
	// Unit(dmg, hp, atkRng, atkSpd)
	Unit archer(1, 4, 5, 3);
	archer.setIcon('@');
	game.characters.push_back(&archer);
	Unit wizard(1, 4, 5, 4);
	wizard.setIcon('%');
	game.characters.push_back(&wizard);
	FiniteStateMachine character1(&game, &archer);
	character1.build();
	for (int i=0; i<10; i++) {
		character1.update();
	}
	return 0;
}
