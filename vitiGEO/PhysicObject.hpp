/* base class for all Object responding to newtonian physics */

/* TO DO: Now that we have the vertices, we can construct the aabb right here */

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Transform.hpp"
#include "AABB.hpp"

namespace vitiGEO {

class PhysicShape;

class PhysicObject {
public:
	PhysicObject	(Transform& transform, 
					AABB* aabb, 
					const std::vector<glm::vec3>& vertices, 
					float mass = 1.0f, 
					const glm::vec3& velocity = {});

	~PhysicObject();

	/* deltaTime: seconds */
	virtual void update(const float& deltaTime);

	void remove();

	/* getters: */
	const glm::vec3& velocity()		const	{ return _v; }
	const glm::vec3& angularVelocity() const { return _av; }
	const glm::vec3& force()		const	{ return _force; }

	float invMass()					const	{ return _massI; }
	const glm::mat3& invInertia()	const	{ return _inertiaI; }

	const glm::quat& orientation()	const	{ return _O; }

	/* access to important member objects: */
	Transform* transform()					{ return &_transform; }
	AABB* aabb()					const	{ return _aabb; }
	PhysicShape* shape()			const	{ return _shape; }

	/* setters: */
	void setVelocity(const glm::vec3& v)	{ _v = v; }
	void setAngularVelocity(const glm::vec3& av) { _av = av; }
	void setForce(const glm::vec3& force)	{ _force = force; }
	void addForce(const glm::vec3& force)	{ _force += force; }

protected:
	Transform&	_transform;	// holds position, rotation and scale

	/* linear */
	glm::vec3	_v;			// velocitiy vector
	glm::vec3	_force;		// the force that is applied on this object
	float		_massI;		// 1 / mass (to avoid lots of divisions)

	/* angular: */
	glm::quat	_O;			// Orientation quaternion
	glm::vec3	_av;		// angular velocity
	glm::vec3	_torque;
	glm::mat3	_inertiaI;	//inverse of the inertia matrix

	/* physic shapes: */
	AABB*		_aabb;		// axis aligned bounding box
	PhysicShape* _shape;	// holds the vertices
};
}

