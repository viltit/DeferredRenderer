/*	Class to represent an axially aligned bounding box (AABB) 

	the class is highly inspired from the book "3D Math Primer for Graphics and Game Development",
	page 302pp, with some differences 

	To be aware of:
	-	Class works with a right handed coordinate system (because OpenGL does too)
*/

#pragma once

#include <vector>

#include <glm/glm.hpp>


namespace vitiGEO {

class Ray;

class AABB {
public:
	AABB();
	AABB(std::vector<glm::vec3>& vertices);
	~AABB();

	/* construct an AABB for a set of given vertices: */
	void construct(std::vector<glm::vec3>& vertices);

	void transform(const glm::mat4& M);

	/* get one of the AABBs eight corners (i has to be in range [0, 7]:) */
	glm::vec3 corner(int i) const;

	/* merge the aabb with another aabb: */
	void add(const AABB& aabb);

	/*	tests for intersection with a ray: 
		returns the fraction of the rays vector to the intersection point:
			intersection = ray.origin + ray.direction * f,
		or a huge number if there is no intersection					*/
	bool rayIntersection(const Ray& ray, glm::vec3& intersection, float& f);

	/*	tests for an intersection with a plane given by the equation N*P - d = 0 
		NOT BUG-TESTES YET	*/
	bool planeIntersection(const glm::vec3& n, float d) const;

	/*	clip a line against one of the three planes of the aabb, return false 
		if there is no intersection, return f-max and f-min as the fractions
		of the intersection points 
		param i: 0 for x-axis, 1 for y-axis, 2 for z-axis				*/
	bool clipLine(int dim, const Ray& ray, float& f_low, float& f_high);

	/*	returns the closest point on the aabb to another point. Returns the original
		point if he is inside the aabb */
	glm::vec3 getClosestPoint (const glm::vec3& point) const;

	/* getters and setters: */
	glm::vec3 min() const { return _min; }
	glm::vec3 max() const { return _max; }

	float diameter() const { return glm::length(_max - _min); }

	void setMin(const glm::vec3& min) { _min = min; }
	void setMax(const glm::vec3& max) { _max = max; }

	glm::vec3 center() const { return (_min + _max) * 0.5f; }

	/* if min is bigger than max, we consider the box to be empty: */
	bool isEmpty() const { return (_min.x > _max.x || _min.y > _max.y || _min.z > _max.z); }

private:
	void addPoint(const glm::vec3& point);

	glm::vec3 _min;
	glm::vec3 _max;

	glm::vec3 _oMin;	//min and max of the original, before any transformations happened
	glm::vec3 _oMax;
};

/* test if two aabbs intersect each other. optionally, give back intersection-aabb: */
bool AABBIntersection(const AABB* box1, const AABB* box2, AABB* intersect = nullptr);

}

