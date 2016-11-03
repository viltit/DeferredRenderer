#include "PhysicObject.hpp"

#include "Math.hpp"
#include "PhysicEngine.hpp"

#include <iostream>

namespace vitiGEO {

PhysicObject::PhysicObject(Transform& transform, float mass, const glm::vec3& velocity)
	:	_transform	{ transform },
		_v			{ velocity },
		_mass		{ mass }
{
	PhysicEngine::addObject(this);
}


PhysicObject::~PhysicObject() {
	PhysicEngine::removeObject(this);
}

/*	very simple newtonian physics: 
	we can add stuff like friction later
*/
void PhysicObject::update(const float& deltaTime) {
	_v += _force * _mass * deltaTime;
	_v += glm::vec3{ 0.01f, 0.01f, 0.01f };

	_transform.move(_v * deltaTime);
}
}