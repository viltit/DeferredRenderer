/* base class for all Object responding to newtonian physics */

/* TO DO: Now that we have the vertices, we can construct the aabb right here */

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Transform.hpp"
#include "AABB.hpp"
#include "PhysicShape.hpp"

namespace vitiGEO {


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

	/* access to important member objects: */
	Transform* transform() { return &_transform; }
	AABB* aabb()					const { return _aabb; }
	IPhysicShape* shape()			const { return _shape; }

	/* getters: */
	const glm::vec3& velocity()		const	{ return _v; }
	const glm::vec3& angularVelocity() const { return _av; }
	const glm::vec3& force()		const	{ return _force; }
	float invMass()					const	{ return _massI; }
	const glm::mat3& invInertia()	const	{ return _inertiaI; }

	float friction()				const	{ return _friction; }

	bool hasG()						const	{ return _hasG; }
	bool isActive()					const	{ return _isActive; }

	/* setters: */
	void setVelocity(const glm::vec3& v)	{ _v = v; }
	void setAngularVelocity(const glm::vec3& av) { _av = av; }
	void setForce(const glm::vec3& force)	{ _force = force; }
	void addForce(const glm::vec3& force)	{ _force += force; }
	void setFriction(float f)				{ _friction = f; }

	void debugDraw()						{ _shape->debugDraw(); }

	void setG(bool g)						{ _hasG = g; }
	void setActive(bool a)					{ _isActive = a; }

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

	float _friction;

	/* physic shapes: */
	AABB*		_aabb;		// axis aligned bounding box
	IPhysicShape* _shape;	// holds the vertices

	/* boolean parameters: */
	bool	_isActive;		// object will still count in collision detection, but will not move itself
	bool	_hasG;			// apply gravity;

};
}

