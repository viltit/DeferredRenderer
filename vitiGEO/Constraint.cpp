#include "Constraint.hpp"

#include "PhysicObject.hpp"

#include <cmath>
#include <assert.h>

namespace vitiGEO {

Constraint::Constraint()
	:	_objA{ nullptr },
		_objB{ nullptr },
		_b	 { 0.0f },
		_delta { 0.0f },
		_softness{ 0.0f },
		_iSumMin{ std::numeric_limits<float>::min() },
		_iSumMax{ std::numeric_limits<float>::max() }
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
		_iSumMin{ std::numeric_limits<float>::min() },
		_iSumMax{ std::numeric_limits<float>::max() }
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
	_iSum = fmin(fmax(_iSum + _delta, _iSum), _iSumMax);
	float realDelta = _iSum - oiSum;

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

	_localA = glm::transpose(glm::mat3_cast(_objA->orientation())) * r1;
	_localB = glm::transpose(glm::mat3_cast(_objB->orientation())) * r2;
}

void DistanceConstraint::preSolver(float deltaTime) {
	/* get the global coordinates of the constraints end point: */
	glm::vec3 r1 = glm::mat3_cast(_objA->orientation()) * _localA;
	glm::vec3 r2 = glm::mat3_cast(_objB->orientation()) * _localB;

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
	float distance_offset = _d - ab.length();
	float baumgarte = 0.1f;

}
}