#pragma once

#include <glm/glm.hpp>
#include <bt/btBulletDynamicsCommon.h>

namespace vitiGEO {

class Picker {
public:
	Picker(btDynamicsWorld* world, const glm::vec3& rayStart, const glm::vec3& rayDir);
	~Picker();

	void onMouseMove(const glm::vec3& camPos);

private:
	btDynamicsWorld*			_world;
	btPoint2PointConstraint*	_p2p;
};
}