#include "PhysicObject.hpp"
#include "Math.hpp"

#include <iostream>

namespace vitiGEO {

PhysicObject::PhysicObject() 
	:	_validW		{ false },
		_mass		{ 0.0f }
{}

PhysicObject::PhysicObject(const glm::vec3 & pos, const glm::vec3 & velocity, float mass) 
	:	_validW		{ false },
		_pos		{ pos },
		_v			{ velocity },
		_mass		{ mass }
{}


PhysicObject::~PhysicObject() {
}

/*	very simple newtonian physics: 
	we can add stuff like friction later
*/
void PhysicObject::update(const float& deltaTime) {
	_v += _force * _mass * deltaTime;
	_v += glm::vec3{ 0.01f, 0.01f, 0.01f };
	_pos += _v * deltaTime;
	_validW = false;
}

const glm::mat4 & PhysicObject::worldMatrix() const {
	if (!_validW) {
		//for later: adjust rotation
		_W = setTranslation(_W, _pos);
		_validW = true;
	}
	return _W;
}

}