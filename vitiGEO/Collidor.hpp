/* Class to handle collision detection in the broad phase and in the narrow phase */

#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace vitiGEO {

const float EPSILON_TRESHOLD = 0.0001f;

class PhysicObject;
class Manifold;

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
	bool AABBIntersect(const PhysicObject* obj1, const PhysicObject* obj2);
	/* separating axis theorem: */
	bool SAT(const PhysicObject* obj1, const PhysicObject* obj2, CollidorData& out);

	void buildManifold	(const PhysicObject* ob1, const PhysicObject* obj2, 
						const CollidorData& collision, 
						Manifold* manifold);

protected:
	void addSATAxis(const glm::vec3& axis, std::vector<glm::vec3>& SATAxes);
	
	bool testSATAxis(const glm::vec3 axis,
					const PhysicObject* obj1, const PhysicObject* obj2,
					CollidorData& out) const;

	void SutherlandHodgesonClipping(
			const std::list<glm::vec3>& inPoly, 
			int numClippingPlanes, 
			const Plane* clippingPlanes, 
			std::list <glm::vec3>& outPoly, 
			bool removePoints);
};

}

