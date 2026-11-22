#include <string>
#include <iostream>
#include <list>

class FSMState;
class FSMTransition;
class FSMStateMachine;
class Unit;

class Waypoint
{
	public:
	Unit* character = nullptr;
	bool isTeam1 = true;
	bool isClimbable = false;
	bool hasLadderOrRope = false;
	bool canBeTerrain = false;
	bool isFlatGround = true;
	bool isFlatWall = false;
	int x = 0;
	int y = 0;
	Waypoint* rightNeighbor = nullptr;
	Waypoint* leftNeighbor = nullptr;
	Waypoint() {};
	Waypoint(Unit* c, bool team, bool climb, bool rope, bool terrain, bool ground, bool wall, int lat, int lon) {
		character = c;
		isTeam1 = team;
		isClimbable = climb;
		hasLadderOrRope = rope;
		canBeTerrain = terrain;
		isFlatGround = ground;
		isFlatWall = wall;
		x = lat;
		y = lon;
	};
	void setRightNeighbor(Waypoint* r) {
		rightNeighbor = r;
	};
	void setleftNeighbor(Waypoint* l) {
		leftNeighbor = l;
	};
	void makeFlatWall() {
		canBeTerrain = true;
		isFlatGround = false;
		isFlatWall = true;
		y = !isFlatGround;
	};
	void makeFlatGround() {
		canBeTerrain = true;
		isFlatGround = true;
		isFlatWall = false;
		y = !isFlatGround;
	};
	void makeRamp() {
		canBeTerrain = true;
		isFlatGround = false;
		isFlatWall = false;
		y = !isFlatGround;
	};
	void makeClimbable() {
		isClimbable = true;
	};
	void addLadderOrRope() {
		if (isClimbable)
			hasLadderOrRope = true;
	};
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
	char icon = '_';
	bool isTeam1 = true;
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
	void setColor(bool nonWhite, bool team) {
		int color = 0;
		if (nonWhite) // 91=r 92=g 93=y 94=b 95=m 96=c
			color = (int)icon%6 + 31 + team*60;
		std::cout << "\033[" << color << "m";
	};
	void setTeam(int team) { isTeam1 = team%2; };
	bool getTeam() { return isTeam1; };
	int getAtkRange() { return atkRange; };
	Waypoint* location;
	// Waypoint* destination;
	/// virtual void getAttackType() { return attackType };
};

class GameState
{
	public:
	std::list<Unit*> characters;
	std::list<FSMStateMachine*> characterFSMs;
	GameState() {};
	Waypoint waypoints[10];
	/// std::list<Waypoint> coordinates;
	bool calculateLOS(bool placeholder) { return placeholder; };
	int distance(Waypoint* start, Waypoint* end) {
		int dist = abs(start->x - end->x);
		if (dist < 0) std::cout << "somehow dist is negative: " << dist << std::endl;
		return dist;
	};
	bool targetInRange(Unit* attacker, Unit* target) { 
		return distance(attacker->location, target->location) < attacker->getAtkRange();
	};
	Unit* getTargetFor(Unit* character) {
		int pos = character->location->x;
		int team1 = character->getTeam();
		int rOrL = character->getTeam() * 2 - 1;
		for (int i=pos; i>0-team1 && i<11-team1; i=i+rOrL) {
			if (waypoints[i].character
				&& waypoints[i].character->getTeam() != character->getTeam())
				return waypoints[i].character;
		}
		// failsafe
		return characters.front();
	};
	void SetWaypoints() {
		for (int xcoord = 0; xcoord<10; xcoord++) {
			waypoints[xcoord].x = xcoord;
		}
		waypoints[3].makeClimbable();
		waypoints[4].makeFlatGround();
		waypoints[5].makeFlatGround();
		waypoints[6].makeClimbable();
	};
	void placeUnitsOnTeams(Unit* u1, Unit* u2, Unit* u3, Unit* u4, Unit* u5, Unit* u6) {
		u1->location = &waypoints[0];
		waypoints[0].character = u1;
		u1->setTeam(1);
		characters.push_back(u1);
		u2->location = &waypoints[1];
		waypoints[1].character = u2;
		u2->setTeam(1);
		characters.push_back(u2);
		u3->location = &waypoints[2];
		waypoints[2].character = u3;
		u3->setTeam(1);
		characters.push_back(u3);
		u4->location = &waypoints[7];
		waypoints[7].character = u4;
		u4->setTeam(2);
		characters.push_back(u4);
		u5->location = &waypoints[8];
		waypoints[8].character = u5;
		u5->setTeam(2);
		characters.push_back(u5);
		u6->location = &waypoints[9];
		waypoints[9].character = u6;
		u6->setTeam(2);
		characters.push_back(u6);
	}
	void draw() {
		for (Unit* u : characters) {
			u->setColor(true, u->getTeam());
			std::cout << u->getIcon();
			u->target->setColor(false, false);
		}
		std::cout << std::endl;
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
			// std::cout << "Entered IdentifyTarget FSMState" << std::endl;
		};
		void onUpdate(GameState* gs, Unit* u) override {
			// std::cout << "Update IdentifyTarget FSMState" << std::endl;
		};
		void onExit(GameState* gs, Unit* u) override {
			// std::cout << "Exit IdentifyTarget FSMState" << std::endl;
		};
};

class ApproachTarget : public FSMState {
	public: 
		void onEnter(GameState* gs, Unit* u) override {
			// std::cout << "Entered ApproachTarget FSMState" << std::endl;
		};
		void onUpdate(GameState* gs, Unit* u) override {
			// std::cout << "Update ApproachTarget FSMState" << std::endl;
			Waypoint* movFromWP = nullptr;
			Waypoint* movToWP = nullptr;
			if ( u->location->x  <  u->target->location->x ) {
				if (!(gs->waypoints[u->location->x + 1].character))
				{
					u->location = &(gs->waypoints[u->location->x + 1]);
					return;
				}
				movFromWP = u->location;
				movToWP = &(gs->waypoints[u->location->x + 1]);
				std::cout << u->getIcon() << " swapped with " << movToWP->character->getIcon() << std::endl;
				u->location = movToWP;
				movToWP->character->location = movFromWP;
				movFromWP->character = movToWP->character;
				movToWP->character = u;
			}
		};
		void onExit(GameState* gs, Unit* u) override {
			// std::cout << "Exit ApproachTarget FSMState" << std::endl;
		};
};

class AttackTarget : public FSMState {
	public: 
		void onEnter(GameState* gs, Unit* u) override {
			u->cooldownAttack();
			// std::cout << "Entered AttackTarget FSMState" << std::endl;
		};
		void onUpdate(GameState* gs, Unit* u) override {
			if (u->getCooldown() > 0) {
				std::cout << u->getIcon() << " cooldown remaining before attacking: ";
				std::cout << u->getCooldown() << std::endl;
				u->decreaseCooldown();
				return;
			}

			u->cooldownAttack();
			// std::cout << "Update AttackTarget FSMState" << std::endl;
			int damage = 2;
			u->target->reduceHealth(damage);
			std::cout << u->getIcon() << " attacked ";
			u->target->setColor(true, u->getTeam());
			std::cout << u->target->getIcon();
			u->setColor(true, u->getTeam());
			std::cout << " dealing " << damage << " damage!" << std::endl;
		};
		void onExit(GameState* gs, Unit* u) override {
			// std::cout << "Exit AttackTarget FSMState" << std::endl;
		};
};

class Dead : public FSMState {
	public: 
		void onEnter(GameState* gs, Unit* u) override {
			// std::cout << "Enter Dead State" << std::endl;
		};
		void onUpdate(GameState* gs, Unit* u) override {
			// std::cout << "Dying Update" << std::endl;
		};
		void onExit(GameState* gs, Unit* u) override {
			// std::cout << "Exit Dead FSMState" << std::endl;
		};
};

class LostTarget : public FSMTransition {
	public:
	LostTarget(FSMState* targetState) : FSMTransition(targetState) {}
	bool isValid(GameState* gs, Unit* u) override { 
		bool noLongerHasTarget = ( u->target == nullptr || u->target->getHealth()<0 );
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
		u->target = gs->getTargetFor(u); // unsure if redundant
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
		unit->setColor(true, unit->getTeam());
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
		unit->setColor(false, unit->getTeam());
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
	// Team 1
	Unit archer(1, 1, 7, 3);
	archer.setIcon('@');

	Unit wizard(1, 4, 6, 4);
	wizard.setIcon('%');

	Unit barbarian(2, 10, 1, 5);
	barbarian.setIcon('#');

	// Team 2
	Unit thief(2, 1, 1, 1);
	thief.setIcon('$');

	Unit mage(1, 4, 7, 4);
	mage.setIcon('/');

	Unit bomber(3, 2, 3, 6);
	bomber.setIcon('!');


	FiniteStateMachine character1(&game, &archer);
	character1.build();

	FiniteStateMachine character2(&game, &wizard);
	character2.build();

	FiniteStateMachine character3(&game, &barbarian);
	character3.build();


	FiniteStateMachine character4(&game, &thief);
	character4.build();

	FiniteStateMachine character5(&game, &mage);
	character5.build();

	FiniteStateMachine character6(&game, &bomber);
	character6.build();
	game.SetWaypoints();
	game.placeUnitsOnTeams(&archer, &wizard, &barbarian, &thief, &mage, &bomber);
	game.draw();

	for (int i=0; i<10; i++) {
		game.draw();
		character1.update();
		character2.update();
		character3.update();
		character4.update();
		character5.update();
		character6.update();
	}
	return 0;
}
