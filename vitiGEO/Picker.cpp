#include "Picker.hpp"
#include "Transform.hpp"

#include <iostream>

namespace vitiGEO {

Picker::Picker(btDynamicsWorld* world, const glm::vec3& rayStart, const glm::vec3& rayDir)
	:	_world{ world },
		_p2p  { nullptr },
		_picked{ nullptr }
{
	assert(_world);
	btVector3 rayS = glmVecToBtVec(rayStart);
	btVector3 rayE = rayS + glmVecToBtVec(rayDir) * 100.0f;

	btCollisionWorld::ClosestRayResultCallback rayCallback(rayS, rayE);
	_world->rayTest(rayS, rayE, rayCallback);

	if (rayCallback.hasHit()) {
		btVector3 pickPos = rayCallback.m_hitPointWorld;
		btRigidBody* body = (btRigidBody*)btRigidBody::upcast(rayCallback.m_collisionObject);
		if (body) {
			if (!(body->isStaticObject() && body->isKinematicObject())) {
				std::cout << "Creating a p2p constraint\n";

				_picked = body;
				_state = body->getActivationState();
				body->setActivationState(DISABLE_DEACTIVATION);
				btVector3 pickPosLocal{ body->getCenterOfMassTransform().inverse() * pickPos };
				_p2p = new btPoint2PointConstraint(*body, pickPosLocal);
				_world->addConstraint(_p2p, true);
				_p2p->m_setting.m_impulseClamp = 3.f;
				_p2p->m_setting.m_tau = 0.001f;
				_p2p->m_setting.m_damping = 0.8f;
			}
		}
		_pickDist = (pickPos - rayS).length();
	}
}


Picker::~Picker() {
	remove();
}


bool Picker::onMouseMove(const glm::vec3& camPos, const glm::vec3& camDir, const SDL_Event& input) {
	bool keepConstraint = true;

	btVector3 rayS{ glmVecToBtVec(camPos) };
	btVector3 rayDir{ glmVecToBtVec(glm::normalize(camDir)) };

	switch (input.type) {
	case SDL_MOUSEMOTION:
		move(rayS, rayDir);
		break;
	case SDL_MOUSEBUTTONUP:
		remove();
		keepConstraint = false;
		std::cout << "Removing p2p constraint\n";
		break;
	}

	return keepConstraint;
}

void Picker::remove() {
	if (_picked) {
		_picked->forceActivationState(_state);
		_picked->activate();
		_picked = nullptr;
		_state = 0;
	}
	if (_p2p) {
		_world->removeConstraint(_p2p);
		delete _p2p;
		_p2p = nullptr;
	}
}

bool Picker::move(const btVector3 & rayStart, const btVector3 & rayDir) {
	if (_p2p) {
		std::cout << "Moving p2p constraint\n";
		btVector3 dir{ rayDir * _pickDist };
		btVector3 pivotPoint{ rayStart + dir };
		std::cout << "Pivot Point: " << pivotPoint.getX() << "/" << pivotPoint.getY() << "/" << pivotPoint.getZ() << std::endl;
		_p2p->setPivotB(pivotPoint);
		return true;
	}

	return false;
}

}