/*	wrapper singleton class to manage all PhysicObjects
	right now, its kind of superfluos, it will be of more use once we have collision detection */

#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "Collidor.hpp"

namespace vitiGEO {

constexpr int SOLVER_ITERATIONS{ 20 };

class PhysicObject;
class Constraint;

struct CollidorPair {
	PhysicObject* objA;
	PhysicObject* objB;
};

class PhysicEngine {
public:
	static PhysicEngine* instance();

	void update(const unsigned int& deltaTime);
	
	void addObject(PhysicObject* obj) { _objects.push_back(obj); }
	void removeObject(PhysicObject* obj);

	void addConstraint(Constraint* c) { _constraints.push_back(c); }
	void removeConstraint(Constraint* c);

	float timestep() { return _timestep; }

	void drawDebug();

protected:
	PhysicEngine();

	void updatePhysics();
	void collisionBroad();	/* brute force at the moment -> to do in future: add octtree */
	void collisionNarrow();	
	void solveConstraint();

	std::vector<PhysicObject*>	_objects;
	std::vector<Constraint*>	_constraints;
	std::vector<Manifold*>		_manifolds;

	std::vector<CollidorPair> _collisionBroad; // possible collision candidates after broad check

	Collidor _collider;

	float _timestep;	/* the timestep each update should take */
	float _timeAccum;	/* accumulated update time */

	glm::vec3 _g;		/* gravity */
};
}
