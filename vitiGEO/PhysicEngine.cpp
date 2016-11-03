#include "PhysicEngine.hpp"

#include "PhysicObject.hpp"

#include <iostream>
#include <algorithm>

namespace vitiGEO {

std::vector<PhysicObject*> PhysicEngine::_objects;

void PhysicEngine::update(const unsigned int& deltaTime) {

	float seconds = float(deltaTime) / 1000.0f;

	for (auto& obj : _objects) {
		obj->update(seconds);
	}
}

void PhysicEngine::removeObject(PhysicObject * obj) {
	_objects.erase(std::remove(_objects.begin(), _objects.end(), obj), _objects.end());
}
}