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
	PhysicEngine::addObject(this);
}


PhysicObject::~PhysicObject() {
	PhysicEngine::removeObject(this);
}

/*	very simple newtonian physics: 
	we can add stuff like friction later
*/
void PhysicObject::update(const float& deltaTime) {
	_v += _force * _massI * deltaTime;
	
	_av = glm::vec3{ 0.1f, 0.1f, 0.1f };
	glm::quat deltaRot = _O * _av;
	_O = _O +  deltaRot;
	_O = glm::normalize(_O);

	_transform.move(_v * deltaTime);
	_transform.rotate(_O);
}
}