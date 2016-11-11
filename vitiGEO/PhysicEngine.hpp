/*	wrapper class to manage all PhysicObjects 
	right now, its kind of superfluos, it will be of more use once we have collision detection */

#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace vitiGEO {

class PhysicObject;

class PhysicEngine {
public:
	static void update(const unsigned int& deltaTime);
	
	static void addObject(PhysicObject* obj) { _objects.push_back(obj); }
	static void removeObject(PhysicObject* obj);

	static float timestep() { return _timestep; }

protected:
	static std::vector<PhysicObject*> _objects;

	static float _timestep;		/* the timestep each update should take */
	static float _timeAccum;	/* accumulated update time */

	static glm::vec3 _g;		/* gravity */
};
}
