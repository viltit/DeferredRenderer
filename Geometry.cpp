#include "Geometry.hpp"

namespace vitiGL {

Plane::Plane()
	:	_distance	{ 0.0f }
{}

Plane::Plane(const glm::vec3 & normal, float distance, bool normalize)
	:	_normal		{ normalize? glm::normalize(normal) : normal },
		_distance	{ normalize? distance / glm::length(normal) : distance }
{}

Plane::~Plane()
{}

bool Plane::sphereInPlane(const glm::vec3 & pos, float radius) const {
	if (glm::dot(pos, _normal) + _distance + radius <= 0) return false;
	return true;
}

}