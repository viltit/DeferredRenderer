/*	Small class for representing a ray 

	(maybe a struct with all public members is enough) */

#pragma once

#include <glm/glm.hpp>

namespace vitiGEO {

class Ray {
public:
	Ray(const glm::vec3& origin, const glm::vec3& delta);
	~Ray();

private:
	glm::vec3 _origin;	//rays point of origin
	glm::vec3 _delta;	//rays length and direction

};
}