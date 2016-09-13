#pragma once

#include "Geometry.hpp"

namespace vitiGL {

class SceneNode;

class Frustum {
public:
	Frustum();
	~Frustum();

	/* constructs the frustum out of the View-Perspective-Matrix */
	void	update(const glm::mat4& VP);
	
	/* tests if a scene node is inside or outside the frustum: */
	bool	isInside(SceneNode& node);

protected:
	Plane p[6];		//the six planes that form the view frustum
};
}
