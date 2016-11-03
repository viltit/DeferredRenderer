/* base class for all Object responding to newtonian physics */

#pragma once

#include <glm/glm.hpp>

#include "Transform.hpp"

namespace vitiGEO {

class PhysicObject {
public:
	PhysicObject(Transform& transform, float mass = 1.0f, const glm::vec3& velocity = {});

	~PhysicObject();

	/* deltaTime: seconds */
	virtual void update(const float& deltaTime);

	/* getters: */
	const glm::vec3& velocity()		const	{ return _v; }
	const glm::vec3& force()		const	{ return _force; }

protected:
	Transform&	_transform;	//holds position, rotation and scale

	glm::vec3	_v;			// velocitiy vector
	glm::vec3	_force;		// the force that is applied on this object
	float		_mass;		// the objects mass

};
}

