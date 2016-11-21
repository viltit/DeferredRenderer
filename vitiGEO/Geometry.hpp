/*	contains classes for geometric representations like planes, and for geometric tests 
	like determing on which side of a plane a sphere is		*/

#pragma once

#include <glm/glm.hpp>

namespace vitiGEO {

/*	Class Plane ------------------------------------------------------------------------
	the plane is represented in the form 
		N*P + d = 0 
	where N is the normal vector of the plane, P the vector to any point on the plane,
	and d is the distance to the coordinates system origin.
*/

class Plane {
public:
	Plane		();
	Plane		(const glm::vec3& normal, float distance, bool normalize = true);
	~Plane		();

	/* test if a sphere is left or right of the plane (relative to (0, 0, 0)) : */
	bool		sphereInPlane(const glm::vec3& pos, float radius) const;

	/* test if a given point lies on the plane: */
	bool		pointInPlane(const glm::vec3& point) const;

	/* perform a plane-edge intersection: */
	glm::vec3	planeEdgeIntersection(const glm::vec3& start, const glm::vec3& end) const;

	/* getters and setters: */
	void		setNormal(const glm::vec3& normal)	{ _normal = normal; }
	glm::vec3	normal() const						{ return _normal; }

	void		setDistance(float d)				{ _distance = d; }
	float		distance() const					{ return _distance; }

protected:
	glm::vec3	_normal;
	float		_distance;

	const float _THRESHOLD = 0.0001f;
};

}