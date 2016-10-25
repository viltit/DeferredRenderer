#include "Ray.hpp"

#include <iostream>

namespace vitiGEO {

Ray::Ray(const glm::vec3& origin, const glm::vec3& delta) 
	:	_origin	{ origin },
		_delta	{ delta }
{}

Ray::~Ray() {
}

bool Ray::rayTriangleIntersect(std::vector<glm::vec3> triangle, glm::vec3 & tuv) {
	/* WHAT GOES WRONG HERE? => The vertices are not scaled, not rotated, not translated! */


	assert(triangle.size() == 3);

	const float epsilon = 0.000001;  //treshold for assuming a value is zero

	/* get two triangles edges: */
	glm::vec3 edge1 = triangle[1] - triangle[0];
	glm::vec3 edge2 = triangle[2] - triangle[0];

	/* normalize the rays delta vector: */
	glm::vec3 delta = glm::normalize(_delta);

	/* get the tripple product of edge1, delta and edge2: */
	glm::vec3 pvec = glm::cross(delta, edge2);
	float det = glm::dot(edge1, pvec);

	/* if tripple product is nearly 0, the ray and the triangle are lying 
	   in parallel planes and we can abort. We can also abort for negative
	   det values, so: */
	if (det < epsilon) return false;

	/* get the vector from vertex0 to rays origin: */
	glm::vec3 vertexToRay = _origin - triangle[0];

	/* we can now calculate the barycentric u-coordinate: */
	tuv.y = glm::dot(vertexToRay, pvec);

	if (tuv.y < 0.0f || tuv.y > det) return false;

	/* similar procedure for the barycentric v-coordinate: */
	glm::vec3 qvec = glm::cross(vertexToRay, edge1);
	tuv.z = glm::dot(delta, qvec);

	if (tuv.z < 0.0f || tuv.z + tuv.y > det) return false;

	/* WE HAVE AN INTERSECTION. Prepare output: */
	tuv.x = glm::dot(edge2, qvec);
	
	float invDet = 1.0f / det;

	tuv.x *= invDet;
	//tuv.x /= _delta.length();
	tuv.y *= invDet;
	tuv.z *= invDet;

	return true;
}
}