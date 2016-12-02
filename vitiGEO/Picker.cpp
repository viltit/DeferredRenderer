#include "Picker.hpp"
#include "Transform.hpp"

#include <iostream>

namespace vitiGEO {

Picker::Picker(btDynamicsWorld* world, const glm::vec3& rayStart, const glm::vec3& rayDir)
	:	_world{ world },
		_p2p  { nullptr }
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
				body->setActivationState(DISABLE_DEACTIVATION);
				btVector3 pickPosLocal{ body->getCenterOfMassTransform().inverse() * pickPos };
				btPoint2PointConstraint* p2p{ new btPoint2PointConstraint(*body, pickPosLocal) };
				_world->addConstraint(p2p, true);
				p2p->m_setting.m_impulseClamp = 30.f;
				p2p->m_setting.m_tau = 0.001f;
			}
		}
	}
}


Picker::~Picker() {
	if (_p2p) {
		delete _p2p;
		_p2p = nullptr;
	}
}


void Picker::onMouseMove(const glm::vec3& camPos) {
	
}
}