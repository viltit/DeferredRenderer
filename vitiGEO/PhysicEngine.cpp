#include "PhysicEngine.hpp"

#include "PhysicObject.hpp"
#include "Constraint.hpp"

#include <iostream>
#include <algorithm>

namespace vitiGEO {

PhysicEngine * PhysicEngine::instance() {
	static PhysicEngine instance;
	return &instance;
}

void PhysicEngine::update(const unsigned int& deltaTime) {
	float seconds = float(deltaTime) / 1000.0f;
	_timeAccum += seconds;

	/*	maintains correctness of differentiation-mathematics, but MAY
		be a problem on very low fps: */
	while (_timeAccum >= _timestep) {
		_timeAccum -= _timestep;

		/* update every object: */
		for (auto& obj : _objects) {
			obj->update(_timestep);
		}
	}
}

void PhysicEngine::removeObject(PhysicObject * obj) {
	_objects.erase(std::remove(_objects.begin(), _objects.end(), obj), _objects.end());
}

void PhysicEngine::removeConstraint(Constraint * c) {
	_constraints.erase(std::remove(_constraints.begin(), _constraints.end(), c), _constraints.end());
}

PhysicEngine::PhysicEngine() {
	_timestep = 1.0f / 60.0f;
	_timeAccum = 0.0f;
	_g = glm::vec3{ 0.0f, -9.81f, 0.0f };
}

void PhysicEngine::solveConstraint() {
	for (Constraint* c : _constraints)
		c->preSolver(_timestep);

	for (size_t i = 0; i < SOLVER_ITERATIONS; i++) {
		for (Constraint* c : _constraints)
			c->addImpulse();
	}
}
}