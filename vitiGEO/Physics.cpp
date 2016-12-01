#include "Physics.hpp"

#include "Physics.hpp"

#include "PhysicObject.hpp"
#include "Transform.hpp"

#include <algorithm>

#define DELETE(X) if(X){ delete X; X = nullptr; }

namespace vitiGEO {


	Physics::Physics() :
		_config{ new btDefaultCollisionConfiguration() },
		_dispatcher{ new btCollisionDispatcher(_config) },
		_broadphase{ new btDbvtBroadphase() },
		_solver{ new btSequentialImpulseConstraintSolver() },
		_world{ new btDiscreteDynamicsWorld(_dispatcher, _broadphase, _solver, _config) }
	{}

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
		_world->stepSimulation(btScalar(deltaTime) / btScalar(60.0f));

		/*	give the new positions and orientations to transform: */
		for (auto& B : _bodies) B->update();
	}

	void Physics::addObject(PhysicObject* obj) {
		_world->addRigidBody(obj->body());
		_bodies.push_back(obj);
	}

	void Physics::removeObject(PhysicObject* obj) {
		_world->removeRigidBody(obj->body());
		_bodies.erase(std::remove(_bodies.begin(), _bodies.end(), obj), _bodies.end());
	}

	void Physics::setGravity(const glm::vec3 & g) {
		_world->setGravity(glmVecToBtVec(g));
	}

	void Physics::rayPick(const glm::vec3& rayStart, const glm::vec3& rayDir) {
		btVector3 rayS = glmVecToBtVec(rayStart);
		btVector3 rayE = rayS + glmVecToBtVec(rayDir) * 100.0f;

		btCollisionWorld::ClosestRayResultCallback rayCallback(rayS, rayE);
		_world->rayTest(rayS, rayE, rayCallback);

		if (rayCallback.hasHit()) {
			std::cout << "HIT!\n";
		}
		else std::cout << "no hit...\n";
	}

}