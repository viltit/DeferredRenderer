#include "PhysicObject.hpp"

#include "Math.hpp"
#include "PhysicEngine.hpp"

#include <iostream>

namespace vitiGEO {

PhysicObject::PhysicObject(Transform& transform, float mass, const glm::vec3& velocity)
	:	_transform	{ transform },
		_v			{ velocity },
		_massI		{ 1.0f / mass },
		_O			{ glm::quat{ }}
{
	/* WIP: Inertia matrix. Right now, we assume everything is a unit length cube: */
	glm::mat3 inertia{};
	inertia[0][0] = mass / 6.0f;
	inertia[1][1] = mass / 6.0f;
	inertia[2][2] = mass / 6.0f;
	_inertiaI = glm::inverse(inertia);

	/* every physic object is part of the engine on construction: */
	PhysicEngine::instance()->addObject(this);
}


PhysicObject::~PhysicObject() {
	PhysicEngine::instance()->removeObject(this);
}

/*	very simple newtonian physics: 
	we can add stuff like friction later
*/
void PhysicObject::update(const float& deltaTime) {
	/* update velocity and position: */
	_v += _force * _massI * deltaTime;
	//add friction here
	_transform.move(_v * deltaTime);

	/* update angular velocity and rotation: */
	glm::vec3 angularAcc = _inertiaI * _torque;
	_av += angularAcc * deltaTime;
	//add friction here

	glm::quat deltaRot = _O * _av;
	_O = _O +  deltaRot;
	_O = glm::normalize(_O);
	_transform.rotate(_O);
}
void PhysicObject::remove() {
	PhysicEngine::instance()->removeObject(this);
}
}