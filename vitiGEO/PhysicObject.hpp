/* base class for all Object responding to newtonian physics */

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

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
	float invMass()					const	{ return _massI; }
	const glm::mat3& invInertia()	const	{ return _inertiaI; }

	/* setters: */
	void setForce(const glm::vec3& force) { _force = force; }
	void addForce(const glm::vec3& force) { _force += force; }

protected:
	Transform&	_transform;	//holds position, rotation and scale

	/* linear */
	glm::vec3	_v;			// velocitiy vector
	glm::vec3	_force;		// the force that is applied on this object
	float		_massI;		// 1 / mass (to avoid lots of divisions)

	/* angular: */
	glm::quat	_O;			// Orientation quaternion
	glm::vec3	_av;		// angular velocity
	glm::vec3	_torque;
	glm::mat3	_inertiaI;	//inverse of the inertia matrix

};
}

