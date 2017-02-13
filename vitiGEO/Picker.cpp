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

				_picked = body;
				_state = body->getActivationState();
				_gravity = body->getGravity();

				body->setActivationState(DISABLE_DEACTIVATION);
				body->setGravity(btVector3{ 0.0f, 0.0f, 0.0f });

				btVector3 pickPosLocal{ body->getCenterOfMassTransform().inverse() * pickPos };
				btTransform transform;
				transform.setIdentity();
				transform.setOrigin(pickPosLocal);
				_pivotA = pickPosLocal;

				_p2p = new btGeneric6DofConstraint(*body, transform, true);
				_p2p->setAngularLowerLimit(btVector3{0.0f, 0.0f, 0.0f});
				_p2p->setAngularUpperLimit(btVector3{ 0.0f, 0.0f, 0.0f });

				/* set constraints strengt (cfm) and error reduction (erp): */
				float cfm = 0.5f;
				float erp = 0.5f;
				for (size_t i = 0; i < 6; i++) {
					_p2p->setParam(BT_CONSTRAINT_STOP_CFM, cfm, i);
					_p2p->setParam(BT_CONSTRAINT_STOP_ERP, erp, i);
				}

				_world->addConstraint(_p2p, true);
			}
		}
		_pickDist = glm::length(btVecToGlmVec(pickPos - rayS));
	}
}

Picker::~Picker() {
	remove();
}


bool Picker::onMouseMove(const glm::vec3& camPos, const glm::vec3& camDir, const SDL_Event& input) {
	bool keepConstraint = true;

	btVector3 rayS{ glmVecToBtVec(camPos) };
	btVector3 rayDir{ glmVecToBtVec(glm::normalize(camDir)) };
	move(rayS, rayDir);

	btTransform t;
	t.setIdentity();

	switch (input.type) {
	case SDL_KEYDOWN:
		switch (input.key.keysym.sym) {
		/* rotate the picked object: */
		case SDLK_1:
			_rotation.setX(_rotation.getX() + 0.1f);
			break;
		case SDLK_2:
			_rotation.setY(_rotation.getY() - 0.1f);
			break;
		case SDLK_3:
			_rotation.setZ(_rotation.getZ() + 0.1f);
			break;
		case SDLK_4:
		{
			btVector3 rayDir = (_picked->getWorldTransform().getOrigin() - rayS).normalize();
			remove(true);
			reset(rayS, rayDir);
		}
			break;
		case SDLK_KP_PLUS:
			_pickDist += 0.1f;
			break;
		case SDLK_KP_MINUS:
			_pickDist -= 0.1f;
			break;
		}
		break;
	case SDL_MOUSEMOTION:
		break;
	case SDL_MOUSEBUTTONUP:
		remove();
		keepConstraint = false;
		break;
	}

	return keepConstraint;
}

void Picker::remove(bool alignToAxis /*= false*/) {
	if (_picked) {
		_picked->forceActivationState(_state);
		_picked->activate();
		_picked->setGravity(_gravity);
		
		/* align to axes */
		if (alignToAxis) {
			btTransform t;
			t = _picked->getWorldTransform();
			t.setRotation(btQuaternion{});
			_picked->setWorldTransform(t);
		}

		_picked = nullptr;
		_state = 0;
	}
	if (_p2p) {
		_world->removeConstraint(_p2p);
		delete _p2p;
		_p2p = nullptr;
	}
	_rotation = btVector3{ 0.0f, 0.0f, 0.0f };
}

bool Picker::move(const btVector3 & rayStart, const btVector3 & rayDir) {
	if (_p2p) {
		btVector3 dir{ rayDir * _pickDist };
		btVector3 pivotPoint{ rayStart + dir };

		_p2p->setAngularLowerLimit(_rotation);
		_p2p->setAngularUpperLimit(_rotation + btVector3{0.01f, 0.01f, 0.01f});
		_p2p->getFrameOffsetA().setOrigin(pivotPoint);
		return true;
	}

	return false;
}

/* reset function is used for aligning object to coordinate axes: */
void Picker::reset(const btVector3 & rayStart, const btVector3& rayD) {

	btVector3 rayS = rayStart;
	btVector3 rayDir = rayD;

	btVector3 rayE = rayS + rayDir * 100.0f;

	btCollisionWorld::ClosestRayResultCallback rayCallback(rayS, rayE);
	_world->rayTest(rayS, rayE, rayCallback);

	if (rayCallback.hasHit()) {
		btVector3 pickPos = rayCallback.m_hitPointWorld;
		btRigidBody* body = (btRigidBody*)btRigidBody::upcast(rayCallback.m_collisionObject);
		if (body) {
			if (!(body->isStaticObject() && body->isKinematicObject())) {

				_picked = body;
				_state = body->getActivationState();
				_gravity = body->getGravity();

				body->setActivationState(DISABLE_DEACTIVATION);
				body->setGravity(btVector3{ 0.0f, 0.0f, 0.0f });

				btVector3 pickPosLocal{ body->getCenterOfMassTransform().inverse() * pickPos };
				btTransform transform;
				transform.setIdentity();
				transform.setOrigin(pickPosLocal);
				_pivotA = pickPosLocal;

				_p2p = new btGeneric6DofConstraint(*body, transform, true);
				_p2p->setAngularLowerLimit(btVector3{ 0.0f, 0.0f, 0.0f });
				_p2p->setAngularUpperLimit(btVector3{ 0.0f, 0.0f, 0.0f });

				std::cout << _rotation.x() << "/" << _rotation.y() << "/" << _rotation.z() << std::endl;

				/* set constraints strengt (cfm) and error reduction (erp): */
				float cfm = 0.5f;
				float erp = 0.5f;
				for (size_t i = 0; i < 6; i++) {
					_p2p->setParam(BT_CONSTRAINT_STOP_CFM, cfm, i);
					_p2p->setParam(BT_CONSTRAINT_STOP_ERP, erp, i);
				}

				_world->addConstraint(_p2p, true);
			}
		}
		_pickDist = glm::length(btVecToGlmVec(pickPos - rayS));
	}

}

}