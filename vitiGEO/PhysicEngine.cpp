#include "PhysicEngine.hpp"

#include "PhysicObject.hpp"
#include "Constraint.hpp"
#include "Manifold.hpp"

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

	for (Manifold* m : _manifolds) {
		m->debugDraw();
	}

	/*	maintains correctness of differentiation-mathematics, but MAY
		be a problem on very low fps: */
	while (_timeAccum >= _timestep) {
		_timeAccum -= _timestep;
		updatePhysics();
	}
}

void PhysicEngine::removeObject(PhysicObject * obj) {
	_objects.erase(std::remove(_objects.begin(), _objects.end(), obj), _objects.end());
}

void PhysicEngine::removeConstraint(Constraint * c) {
	_constraints.erase(std::remove(_constraints.begin(), _constraints.end(), c), _constraints.end());
}

void PhysicEngine::drawDebug() {
	for (const auto& o : _objects) {
		o->debugDraw();
	}
}

PhysicEngine::PhysicEngine() {
	_timestep = 1.0f / 60.0f;
	_timeAccum = 0.0f;
	_g = glm::vec3{ 0.0f, -9.81f, 0.0f };
}

void PhysicEngine::updatePhysics() {

	for (Manifold* m : _manifolds) {
		delete m;
	}
	_manifolds.clear(),

	collisionBroad();
	collisionNarrow();

	solveConstraint();

	/* update every object: */
	for (auto& obj : _objects) {
		obj->update(_timestep);
	}
}

void PhysicEngine::collisionBroad() {
	_collisionBroad.clear();

	for (size_t i = 0; i < _objects.size() - 1; i++) {
		for (size_t j = i + 1; j < _objects.size(); j++) {
			if (_collider.AABBIntersect(_objects[i], _objects[j])) {
				_collisionBroad.push_back(CollidorPair{ _objects[i], _objects[j] });
			}
		}
	}
}

void PhysicEngine::collisionNarrow() {
	for (size_t i = 0; i < _collisionBroad.size(); i++) {
		CollidorData data;
		if (_collider.SAT(_collisionBroad[i].objA, _collisionBroad[i].objB, data)) {
			
			/*	here would be the place to call a "bool onCollisionCallback()" function
				in PhysicObject to implement special cases or to ignore the collision */
			/**/
			Manifold* manifold = new Manifold{ _collisionBroad[i].objA, _collisionBroad[i].objB };
			_collider.buildManifold(_collisionBroad[i].objA, _collisionBroad[i].objB, data, manifold);
			_manifolds.push_back(manifold);
		}
	}
}

void PhysicEngine::solveConstraint() {
	for (Manifold* m : _manifolds) {
		m->preSolver(_timestep);
	}

	for (Constraint* c : _constraints)
		c->preSolver(_timestep); 


	for (size_t i = 0; i < SOLVER_ITERATIONS; i++) {
		/*
		for (Manifold* m : _manifolds) {
			m->applyImpulse();
		}*/

		for (Constraint* c : _constraints)
			c->addImpulse();
	}
}
}