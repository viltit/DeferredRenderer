#include "Physics.hpp"

#include "Physics.hpp"

#include "PhysicObject.hpp"
#include "Constraints.hpp"
#include "Transform.hpp"
#include "Picker.hpp"

#include <algorithm>

#define DELETE(X) if(X){ delete X; X = nullptr; }

namespace vitiGEO {


Physics::Physics() :
	_config{ new btDefaultCollisionConfiguration() },
	_dispatcher{ new btCollisionDispatcher(_config) },
	_broadphase{ new btDbvtBroadphase() },
	_solver{ new btSequentialImpulseConstraintSolver() },
	_world{ new btDiscreteDynamicsWorld(_dispatcher, _broadphase, _solver, _config) },
	_drawDebug(false),
	_isOn { true }
{ }

Physics * Physics::instance() {
	static Physics instance{};
	return &instance;
}

Physics::~Physics() {
	DELETE(_world);
	DELETE(_solver);
	DELETE(_broadphase);
	DELETE(_dispatcher);
	DELETE(_config);
}

void Physics::update(unsigned int deltaTime) {
	if (_isOn) {
		_world->stepSimulation(btScalar(deltaTime) / btScalar(1000.0f));
	

	/* wip: get collision pairs... 
	for (size_t i = 0; i < _world->getNumCollisionObjects(); i++) {
		btRigidBody* body{ btRigidBody::upcast(_world->getCollisionObjectArray()[i]) };
	}*/

	/*	give the new positions and orientations to transform: */
		for (auto& B : _bodies) B->update();
	}

	if (_drawDebug) {
		for (auto& B : _bodies) {
			btTransform t;
			B->body()->getMotionState()->getWorldTransform(t);
			_world->debugDrawObject(t, B->body()->getCollisionShape(), btVector3{ 1.0f, 0.0f, 0.0f });
		}
		for (auto& C : _constraints) {
			_world->debugDrawConstraint(C->obj());
		}
	}
}

void Physics::update(const glm::vec3& camPos, const glm::vec3& camDir, const SDL_Event& input) {
	if (_picker) {
		if (!_picker->onMouseMove(camPos, camDir, input)) {
			delete _picker;
			_picker = nullptr;
		}
	}
}

void Physics::reset() {
	DELETE(_world);
	DELETE(_solver);
	DELETE(_broadphase);
	DELETE(_dispatcher);
	DELETE(_config);

	_config = new btDefaultCollisionConfiguration();
	_dispatcher = new btCollisionDispatcher(_config);
	_broadphase = new btDbvtBroadphase();
	_solver = new btSequentialImpulseConstraintSolver();
	_world = new btDiscreteDynamicsWorld(_dispatcher, _broadphase, _solver, _config);
}

void Physics::addObject(PhysicObject* obj) {
	_world->addRigidBody(obj->body());
	_bodies.push_back(obj);
}

void Physics::removeObject(PhysicObject* obj) {
	_world->removeRigidBody(obj->body());
	_bodies.erase(std::remove(_bodies.begin(), _bodies.end(), obj), _bodies.end());
}

void Physics::addConstraint(Constraint * c) {
	_world->addConstraint(c->obj());
	_constraints.push_back(c);
}

void Physics::removeConstraint(Constraint * c) {
	_world->removeConstraint(c->obj());
	_constraints.erase(std::remove(_constraints.begin(), _constraints.end(), c), _constraints.end());
}

void Physics::setGravity(const glm::vec3 & g) {
	_world->setGravity(glmVecToBtVec(g));
}

void Physics::picker(const glm::vec3& rayStart, const glm::vec3& rayDir) {
	_picker = new Picker{ _world, rayStart, rayDir };
	if (!_picker->hasBody()) {
		delete _picker;
		_picker = nullptr;
	}
}

}