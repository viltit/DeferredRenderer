/*	
	Class Manifold 
	The manifold stores the surface area of a collision between two objects. It also holds
	a series of constraints which should solve the collission.
*/

#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace vitiGEO {

class PhysicObject;
class Constraint;

struct Contact {
	Contact(const glm::vec3& relPosA, const glm::vec3& relPosB, 
			const glm::vec3& normal, float depth) 
		
		:	relPosA { relPosA },
			relPosB { relPosB },
			cNormal { normal },
			depth   { depth }
	{}

	Constraint* normal;		// normal force constraint along the collision normal
	Constraint* frictionA;	// friction force constraint along the contact plane (eg. perpendicular to the normal)
	Constraint* frictionB;

	glm::vec3 cNormal;
	float depth;

	glm::vec3 relPosA;
	glm::vec3 relPosB;
};

class Manifold {
public:
	Manifold(PhysicObject* A, PhysicObject* B);
	~Manifold();

	/* add a new collison contact between A and B: */
	void addContact(const glm::vec3& globalOnA, 
					const glm::vec3& globalOnB, 
					const glm::vec3 normal, 
					float depth);

	/* solve each contact constraint: */
	void applyImpulse();
	void preSolver(float deltaTime);


	/* debug draw the manifold surface: */
	void debugDraw() const;

	PhysicObject* objA() { return _objA; }
	PhysicObject* objB() { return _objB; }

protected:
	void updateConstraint(Contact& c);

	PhysicObject* _objA;
	PhysicObject* _objB;

	std::vector<Contact> _contacts;
};
}
