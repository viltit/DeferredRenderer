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

	glm::vec3 toEuler(const glm::quat q) {
		double roll, pitch, yaw;

		double ysqr = q.y * q.y;

		// roll (x-axis rotation)
		double t0 = +2.0 * (q.w * q.x + q.y * q.z);
		double t1 = +1.0 - 2.0 * (q.x * q.x + ysqr);
		roll = std::atan2(t0, t1);

		// pitch (y-axis rotation)
		double t2 = +2.0 * (q.w * q.y - q.z * q.x);
		t2 = t2 > 1.0 ? 1.0 : t2;
		t2 = t2 < -1.0 ? -1.0 : t2;
		pitch = std::asin(t2);

		// yaw (z-axis rotation)
		double t3 = +2.0 * (q.w * q.z + q.x * q.y);
		double t4 = +1.0 - 2.0 * (ysqr + q.z * q.z);
		yaw = std::atan2(t3, t4);

		return glm::vec3{ roll, pitch, yaw };
	}

	btVector3 toEuler(const btQuaternion & q) {
		double roll, pitch, yaw;

		double ysqr = q.y() * q.y();

		// roll (x-axis rotation)
		double t0 = +2.0 * (q.w() * q.x() + q.y() * q.z());
		double t1 = +1.0 - 2.0 * (q.x() * q.x() + ysqr);
		roll = std::atan2(t0, t1);

		// pitch (y-axis rotation)
		double t2 = +2.0 * (q.w() * q.y() - q.z() * q.x());
		t2 = t2 > 1.0 ? 1.0 : t2;
		t2 = t2 < -1.0 ? -1.0 : t2;
		pitch = std::asin(t2);

		// yaw (z-axis rotation)
		double t3 = +2.0 * (q.w() * q.z() + q.x() * q.y());
		double t4 = +1.0 - 2.0 * (ysqr + q.z() * q.z());
		yaw = std::atan2(t3, t4);

		return btVector3{ btScalar(roll), btScalar(pitch), btScalar(yaw) };;
	}
}