/*	---------------------------------------------------------------------------------
	A constraint imposes limits to the 6 degrees of freedom a rigid body has,
	for example by defining a distance two objects should have between them. It
	works by adding a force to the object which moves them within the constraints
	boundaries.
	
	This is the abstract base class for different kind of constraints, like
	a distance constraint or a friction constraint

	I closely follow the Physics Engine Tutorial of the Newcastle University:
	https://research.ncl.ac.uk/game/mastersdegree/gametechnologies/

	(I honestly only understand 50% of the math here...)
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

	/*	the solver needs to be adapted to the type of the constraint: 
		it needs to be run BEFORE PhysicObject::update() */
	virtual void preSolver(float deltaTime) = 0;

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


/* -------------------------- The distance Constraint ----------------------------- */
class DistanceConstraint : public Constraint {
public:
	/*	globalA and globalB: The global coordinates on the surface points on a and b 
		on which the distance constraint is applied. */
	DistanceConstraint	(PhysicObject* a, PhysicObject* b,
						const glm::vec3& globalA, const glm::vec3 globalB);


	/* delta time in seconds */
	virtual void preSolver(float deltaTime) override;

protected:
	float _d;			//the distance that must be kept
	glm::vec3 _localA;	//the local positon of the surface-points
	glm::vec3 _localB;
};



}