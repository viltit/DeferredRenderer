#include "PhysicObject.hpp"

#include "Math.hpp"
#include "PhysicEngine.hpp"

#include <iostream>

namespace vitiGEO {

PhysicObject::PhysicObject(Transform& transform, AABB* aabb, const std::vector<glm::vec3>& vertices, 
	float mass, const glm::vec3& velocity)
	:	_transform	{ transform },
		_v			{ velocity },
		_massI		{ 1.0f / mass },
		_O			{ glm::quat{ }},
		_aabb		{ aabb },
		_friction	{ 0.01f }
{
	assert(_aabb);

	/* create the shape: 
		TO DO: switch depending on shape type */
	_shape = new CuboidShape{ this };

	_inertiaI = _shape->inverseInertia(_massI);

	/* every physic object is part of the engine on construction: */
	PhysicEngine::instance()->addObject(this);
}


PhysicObject::~PhysicObject() {
	PhysicEngine::instance()->removeObject(this);
	if (_shape) {
		delete _shape;
		_shape = nullptr;
	}
}

/*	very simple newtonian physics: 
	we can add stuff like friction later
*/
void PhysicObject::update(const float& deltaTime) {
	/* update velocity and position: */
	_v += _force * _massI * deltaTime;
	//_v /= _friction;
	_transform.move(_v * deltaTime);

	/* update angular velocity and rotation: */
	glm::vec3 angularAcc = _inertiaI * _torque;
	_av += angularAcc * deltaTime;

	glm::quat deltaRot = _O * _av * deltaTime * 0.5f;
	_O = _O +  deltaRot;
	_O = glm::normalize(_O);
	_transform.rotate(_O);
}

void PhysicObject::remove() {
	PhysicEngine::instance()->removeObject(this);
}

}