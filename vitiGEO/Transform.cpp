#include "Transform.hpp"

namespace vitiGEO {
	btQuaternion glmQuatToBtQuat(const glm::quat& q) {
		return btQuaternion{ q.x, q.y, q.z, q.w };
	}

	btVector3 glmVecToBtVec(const glm::vec3& v) {
		return btVector3{ v.x, v.y, v.z };
	}

	glm::quat btQuatToGlmQuat(const btQuaternion& q) {
		return glm::quat{ q.getW(), q.getX(), q.getY(), q.getZ() };
	}

	glm::vec3 btVecToGlmVec(const btVector3& v) {
		return glm::vec3{ v.getX(), v.getY(), v.getZ() };
	}
}