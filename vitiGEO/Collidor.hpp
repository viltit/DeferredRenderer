/* Class to handle collision detection in the broad phase and in the narrow phase */

#pragma once

#include <glm/glm.hpp>

namespace vitiGEO {

class PhysicObject;

struct CollidorData {
	glm::vec3 hitPoint;
	glm::vec3 hitNormal;
	float depth;
};

class Collidor {
public:
	Collidor();
	~Collidor();

	/* broad phase check: */
	bool AABBIntersection(const PhysicObject* obj1, const PhysicObject* obj2);

	/* separating axis theorem: */
	bool SAT(const PhysicObject* obj1, const PhysicObject* obj2, CollidorData& out);

protected:
};

}

