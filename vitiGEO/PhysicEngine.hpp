/*	wrapper class to manage all PhysicObjects 
	right now, its kind of superfluos, it will be of more use once we have collision detection */

#pragma once

#include <vector>

namespace vitiGEO {

class PhysicObject;

class PhysicEngine {
public:

	void update(const unsigned int& deltaTime);
	
	static void addObject(PhysicObject* obj) { _objects.push_back(obj); }
	static void removeObject(PhysicObject* obj);

protected:
	static std::vector<PhysicObject*> _objects;
};
}
