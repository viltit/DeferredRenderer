#include "Geometry.hpp"

namespace vitiGEO {

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

bool Plane::pointInPlane(const glm::vec3 & point) const {
	if (fabs(glm::dot(_normal, point) + _distance) < _THRESHOLD) return true;
	return false;
}

glm::vec3 Plane::planeEdgeIntersection(const glm::vec3 & start, const glm::vec3 & end) const {
	float startDist = glm::dot(start, _normal) + _distance;
	float endDist = glm::dot(end, _normal) + _distance;

	glm::vec3 ab = end - start;	
	float abP = glm::dot(_normal, ab);

	if (fabs(abP) > _THRESHOLD) {
		glm::vec3 pCo = _normal * -_distance;
		glm::vec3 w = start - pCo;
		float fac = -glm::dot(_normal, w) / abP;
		ab = ab * fac;
		return start + ab;
	}

	return start;
}

}