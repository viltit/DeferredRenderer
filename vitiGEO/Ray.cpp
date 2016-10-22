#include "Ray.hpp"

namespace vitiGEO {

Ray::Ray(const glm::vec3& origin, const glm::vec3& delta) 
	:	_origin	{ origin },
		_delta	{ delta }
{}

Ray::~Ray() {
}
}