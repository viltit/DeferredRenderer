#include "PhysicEngine.hpp"

#include "PhysicObject.hpp"

#include <iostream>

namespace vitiGEO {

std::vector<PhysicObject*> PhysicEngine::_objects;

void PhysicEngine::update(const unsigned int& deltaTime) {

	float seconds = float(deltaTime) / 1000.0f;

	for (auto& obj : _objects) {
		obj->update(seconds);
	}
}
}