/*	A constraint imposes limits to the 6 degrees of freedom a rigid body has,
	for example by defining a distance two objects should have between them		
	
	This is the abstract base class for different kind of constraints, like
	a distance constraint or a friction constraint
*/

#pragma once

#include <glm/glm.hpp>

namespace vitiGEO {

class PhysicObject;

class Constraint {
public:
	Constraint();
	Constraint	(PhysicObject* objA, PhysicObject* objB,
				const glm::vec3& j1, const glm::vec3& j2,
				const glm::vec3& j3, const glm::vec3& j4,
				float b);

	virtual ~Constraint();

	void addImpulse();

	virtual void solver(float deltaTime) = 0;

protected:
	PhysicObject* _objA;
	PhysicObject* _objB;

	glm::vec3 _j1, _j2, _j3, _j4;	// Jacobinian diagonals
	float _b;						// bias
		
	float _delta;					// constraints delta

	float _iSum;					// accumulated delta
	float _iSumMin;
	float _iSumMax;

	float _softness;
};

}