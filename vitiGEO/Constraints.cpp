#include "Constraints.hpp"

#include "PhysicObject.hpp"
#include "Transform.hpp"
#include "Physics.hpp"

namespace vitiGEO {

Constraint::Constraint(ConstraintType type, const PhysicObject* objA, const glm::vec3& pivotA,
					   const PhysicObject* objB, const glm::vec3& pivotB) 
	: _constraint{ nullptr }
{
	btVector3 pivot1 = glmVecToBtVec(pivotA);
	btVector3 pivot2 = glmVecToBtVec(pivotB);

	switch (type) {
	case ConstraintType::p2p:
		if (objA && !objB) makeSingleP2P(objA, pivot1);
		else if (objA && objB) makeP2P(objA, pivot1, objB, pivot2);
		break;
	}
}

Constraint::~Constraint() { 
	remove();
}

void Constraint::remove() {
	if (_constraint) {
		Physics::instance()->removeConstraint(this);
		delete _constraint;
		_constraint = nullptr;
	}
}

void Constraint::makeP2P(const PhysicObject * objA, const btVector3& pivotA, const PhysicObject * objB, const btVector3& pivotB) {
	btPoint2PointConstraint* p2p = new btPoint2PointConstraint(*objA->body(), *objB->body(), pivotA, pivotB);
	_constraint = p2p;
	Physics::instance()->addConstraint(this);
}

void Constraint::makeSingleP2P(const PhysicObject * objA, const btVector3& pivotPoint) {
	btPoint2PointConstraint* p2p = new btPoint2PointConstraint(*objA->body(), pivotPoint);
	_constraint = p2p;
	Physics::instance()->addConstraint(this);
}

}