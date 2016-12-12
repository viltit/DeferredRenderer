/* Wrapper class to access btConstraints */

#pragma once

#include <bt/btBulletCollisionCommon.h>
#include <bt/btBulletDynamicsCommon.h>

#include <glm/glm.hpp>

namespace vitiGEO {

class PhysicObject;

enum class ConstraintType {
	p2p
};

class Constraint {
public:
	Constraint(ConstraintType type, 
			   const PhysicObject* objA, const glm::vec3& pivotA,
			   const PhysicObject* objB = nullptr, const glm::vec3& pivotB = {});
	~Constraint();

	void remove();

	/* access btConstraint: */
	btTypedConstraint* obj() { return _constraint; }

private:
	void makeP2P(const PhysicObject* objA, const btVector3& pivotA,
				 const PhysicObject* objB, const btVector3& pivotB);

	void makeSingleP2P(const PhysicObject* objA, const btVector3& pivotPoint);

	btTypedConstraint* _constraint;
};
}

