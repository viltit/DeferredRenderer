/*	Small class for representing a ray 

	(maybe a struct with all public members is enough) */

#pragma once

#include <vector>

#include <glm/glm.hpp>

namespace vitiGEO {

struct Ray {
public:
	Ray(const glm::vec3& origin, const glm::vec3& delta);
	~Ray();

	/*	Ray-Triangle-Intersection test according to the Möller-Trumbore algorithm
		see 
		http://www.cs.virginia.edu/~gfx/Courses/2003/ImageSynthesis/papers/Acceleration/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf
		
		triangle: The three vertices of the triangle
		tuv: The functions output:
			t - Parametric value of the ray intersection point:
				intersection = ray.origin + t * ray.delta
			u, v - The barycentric coordinates of the intersection point
	*/
	bool rayTriangleIntersect(std::vector<glm::vec3> triangle, glm::vec3& tuv);

	glm::vec3 _origin;	//rays point of origin
	glm::vec3 _delta;	//rays length and direction

};
}