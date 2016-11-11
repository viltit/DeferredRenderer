#include "PhysicEngine.hpp"

#include "PhysicObject.hpp"

#include <iostream>
#include <algorithm>

namespace vitiGEO {

std::vector<PhysicObject*> PhysicEngine::_objects;
float PhysicEngine::_timestep = 1.0f / 60.0f;
float PhysicEngine::_timeAccum = 0.0f;
glm::vec3 PhysicEngine::_g = glm::vec3{ 0.0f, -9.81f, 0.0f };


void PhysicEngine::update(const unsigned int& deltaTime) {
	float seconds = float(deltaTime) / 1000.0f;
	_timeAccum += seconds;

	/*	maintains correctness of differentiation-mathematics, but MAY
		be a problem on very low fps: */
	while (_timeAccum >= _timestep) {
		_timeAccum -= _timestep;

		/* update every object: */
		for (auto& obj : _objects) {
			obj->update(_timestep);
		}
	}
}

void PhysicEngine::removeObject(PhysicObject * obj) {
	_objects.erase(std::remove(_objects.begin(), _objects.end(), obj), _objects.end());
}
}