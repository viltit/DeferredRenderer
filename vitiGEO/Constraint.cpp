#include "Constraint.hpp"

#include "PhysicObject.hpp"
#include "PhysicEngine.hpp"

#include <cmath>
#include <assert.h>

namespace vitiGEO {

Constraint::Constraint()
	:	_objA{ nullptr },
		_objB{ nullptr },
		_b	 { 0.0f },
		_delta { 0.0f },
		_softness{ 0.0f },
		_iSum { 0.0f },
		_iSumMin{ -FLT_MAX },
		_iSumMax{ FLT_MAX }
{}

Constraint::Constraint(PhysicObject * objA, PhysicObject * objB, 
	const glm::vec3 & j1, const glm::vec3 & j2, const glm::vec3 & j3, const glm::vec3 & j4, float b) 
	:	_objA{ objA },
		_objB{ objB },
		_b{ b},
		_j1{ j1 },
		_j2{ j2 },
		_j3{ j3 },
		_j4{ j4 },
		_delta{ 0.0f },
		_softness{ 0.0f },
		_iSum{ 0.0f },
		_iSumMin{ -FLT_MAX },
		_iSumMax{ FLT_MAX }
{}


Constraint::~Constraint() {
}

void Constraint::addImpulse() {
	_delta = 0.0f;

	/* J * M(-1) * J(t) */
	float constraintMass =
		_objA->invMass() * glm::dot(_j1, _j1)
		+ glm::dot(_j2, (_objA->invInertia() * _j2))
		+ _objB->invMass() * glm::dot(_j3, _j3)
		+ glm::dot(_j4, (_objB->invInertia(), _j4))
		+ _softness;

	/* J * V (jacobian * velocity) */
	if (constraintMass > 0.00001f) {
		float jv = 
			glm::dot(_j1, _objA->velocity())
			+ glm::dot(_j2, _objA->angularVelocity())
			+ glm::dot(_j3, _objB->velocity())
			+ glm::dot(_j4, _objB->angularVelocity());

		float denom = -(jv + _b);

		/* finally, our delta value: */
		_delta = denom / constraintMass;
	}

	float oiSum = _iSum;
	_iSum = fmin(fmax(_iSum + _delta, _iSumMin), _iSumMax);
	//_iSum += _delta;
	float realDelta = _iSum - oiSum;


	//float realDelta = _delta;

	/*if (glm::length(v1) > 0.01) {
		std::cout << "obj A velocity: " << v1.x << "/" << v1.y << "/" << v1.z << std::endl;
		std::cout << "Real delta: " << realDelta << std::endl;
		std::cout << "j1: " << _j1.x << "/" << _j1.y << "/" << _j1.z << std::endl;
		std::cout << "delta: " << _delta << std::endl;
		std::cout << "constraint mass: " << constraintMass << std::endl;
	}*/

	glm::vec3 v1 = _objA->velocity() + (_j1 * realDelta * _objA->invMass());
	glm::vec3 a1 = _objA->angularVelocity() + (_objA->invInertia() * _j2 * realDelta);
	glm::vec3 v2 = _objB->velocity() + (_j3 * realDelta * _objB->invMass());
	glm::vec3 a2 = _objB->angularVelocity() + (_objB->invInertia() * _j4 * realDelta);

	_objA->setVelocity(_objA->velocity() + (_j1 * realDelta * _objA->invMass()));
	_objA->setAngularVelocity(_objA->angularVelocity() + (_objA->invInertia() * _j2 * realDelta));
	_objB->setVelocity(_objB->velocity() + (_j3 * realDelta * _objB->invMass()));
	_objB->setAngularVelocity(_objB->angularVelocity() + (_objB->invInertia() * _j4 * realDelta));
}


/* -------------------------- The distance Constraint ----------------------------- */
DistanceConstraint::DistanceConstraint(PhysicObject * a, PhysicObject * b, 
	const glm::vec3 & globalA, const glm::vec3 globalB) 

	:	Constraint()
{
	assert(a && b);
	_objA = a;
	_objB = b;

	/* get the distance: */
	glm::vec3 ab = globalB - globalA;
	_d = glm::length(ab);

	/* get the local position of the constraints end points: */
	glm::vec3 r1 = globalA - _objA->transform()->pos();
	glm::vec3 r2 = globalB - _objB->transform()->pos();

	_localA = glm::transpose(glm::toMat3(_objA->orientation())) * r1;
	_localB = glm::transpose(glm::toMat3(_objB->orientation())) * r2;
}

void DistanceConstraint::preSolver(float deltaTime) {
	//_iSum = 0.0f;

	/* get the global coordinates of the constraints end point: */
	glm::vec3 r1 = glm::toMat3(_objA->orientation()) * _localA;
	glm::vec3 r2 = glm::toMat3(_objB->orientation()) * _localB;

	glm::vec3 globalA = _objA->transform()->pos() + r1;
	glm::vec3 globalB = _objB->transform()->pos() + r2;

	/* get vector from objA to objB: */
	glm::vec3 ab = globalB - globalA;
	glm::vec3 abn = glm::normalize(ab);

	/* calculate the jacobian: */
	_j1 = -abn;
	_j2 = glm::cross(-r1, abn);
	_j3 = abn;
	_j4 = glm::cross(r2, abn);

	/* so called Baumgarte Offset to counter solving errors: */
	float distanceOffset = _d - glm::length(ab);
	float baumgarte = 0.1f;
	_b = -(baumgarte / deltaTime * distanceOffset);
}
}