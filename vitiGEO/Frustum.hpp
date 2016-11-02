#pragma once

#include "Geometry.hpp"

namespace vitiGEO {

class Frustum {
public:
	Frustum();
	~Frustum();

	/* constructs the frustum out of the View-Perspective-Matrix */
	void	update(const glm::mat4& VP);
	
	/* tests if a sphere is inside or outside the frustum: */
	bool	isInside(const glm::vec3& pos, float r);

protected:
	Plane p[6];		//the six planes that form the view frustum
};
}
