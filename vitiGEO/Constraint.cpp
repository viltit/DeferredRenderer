#include "Constraint.hpp"

#include "PhysicObject.hpp"

#include <assert.h>

namespace vitiGEO {

Constraint::Constraint()
	:	_objA{ nullptr },
		_objB{ nullptr },
		_b	 { 0.0f },
		_delta { 0.0f },
		_softness{ 0.0f },
		_iSum{ 0.0f },
		_iSumMin{ 0.0f }
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
		_iSumMin{ 0.0f }
{}


Constraint::~Constraint() {
}

void Constraint::addImpulse() {
	assert(_objA && _objB);

	_delta = 0.0f;

	/* J * M(-1) * J(t) */
	float constraintMass =
		_objA->invMass() * glm::dot(_j1, _j1)
		+ glm::dot(_j2, (_objA->invInertia() * _j2))
		+ _objB->invMass() * glm::dot(_j3, _j3)
		+ glm::dot(_j4, (_objB->invInertia(), _j4))
		+ _softness;

	/* J * V */
	if (constraintMass > 0.00001f) {

	}
}


}