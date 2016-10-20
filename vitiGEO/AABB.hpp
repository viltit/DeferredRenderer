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

	/* getters and setters: */
	glm::vec3 min() const { return _min; }
	glm::vec3 max() const { return _max; }

	/* if min is bigger than max, we consider the box to be empty: */
	bool isEmpty() const { return (_min.x > _max.x || _min.y > _max.y || _min.z > _max.z); }

private:
	void addPoint(const glm::vec3& point);

	glm::vec3 _min;
	glm::vec3 _max;

	/* wip: */
	glm::vec3 oldT;	//previous translation
};
}

