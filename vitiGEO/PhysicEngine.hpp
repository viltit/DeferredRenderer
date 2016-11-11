/*	wrapper singleton class to manage all PhysicObjects
	right now, its kind of superfluos, it will be of more use once we have collision detection */

#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace vitiGEO {

constexpr int SOLVER_ITERATIONS{ 20 };

class PhysicObject;
class Constraint;

class PhysicEngine {
public:
	static PhysicEngine* instance();

	void update(const unsigned int& deltaTime);
	
	void addObject(PhysicObject* obj) { _objects.push_back(obj); }
	void removeObject(PhysicObject* obj);

	void addConstraint(Constraint* c) { _constraints.push_back(c); }
	void removeConstraint(Constraint* c);

	float timestep() { return _timestep; }

protected:
	PhysicEngine();

	void solveConstraint();

	std::vector<PhysicObject*> _objects;
	std::vector<Constraint*> _constraints;

	float _timestep;	/* the timestep each update should take */
	float _timeAccum;	/* accumulated update time */

	glm::vec3 _g;		/* gravity */
};
}
