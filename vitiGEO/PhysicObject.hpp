/* base class for all Object responding to newtonian physics */

#pragma once

#include <glm/glm.hpp>

namespace vitiGEO {

class PhysicObject {
public:
	PhysicObject();
	PhysicObject(const glm::vec3& pos, const glm::vec3& velocity, float mass);

	~PhysicObject();

	/* deltaTime: seconds */
	virtual void update(const float& deltaTime);

	/* getters: */
	const glm::vec3& pos()			const	{ return _pos; }
	const glm::vec3& velocity()		const	{ return _v; }
	const glm::vec3& force()		const	{ return _force; }
	
	const glm::mat4& worldMatrix()	const;

protected:
	mutable glm::mat4	_W;	// world position Matrix
	mutable bool _validW;	// marks if W needs to be updated

	/* linear: */
	glm::vec3	_pos;		// world position vector
	glm::vec3	_v;			// velocitiy vector
	glm::vec3	_force;		// the force that is applied on this object
	float		_mass;		// the objects mass

};
}

