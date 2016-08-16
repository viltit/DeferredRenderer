#include "Frustum.hpp"

namespace vitiGL {

Frustum::Frustum() {}

Frustum::~Frustum() {}

void Frustum::update(const glm::mat4 & VP) {
	glm::vec3 x_axis = glm::vec3(VP[0][0], VP[1][0], VP[2][0]);
	glm::vec3 y_axis = glm::vec3(VP[0][1], VP[1][1], VP[2][1]);
	glm::vec3 z_axis = glm::vec3(VP[0][2], VP[1][2], VP[2][2]);
	glm::vec3 w_axis = glm::vec3(VP[0][3], VP[1][3], VP[2][3]);

	p[0] = Plane(w_axis - x_axis, VP[3][3] - VP[3][0]);		/* right p */
	p[1] = Plane(w_axis + x_axis, VP[3][3] + VP[3][0]);		/* left */
	p[2] = Plane(w_axis + y_axis, VP[3][3] + VP[3][1]);		/* bottom */
	p[3] = Plane(w_axis - y_axis, VP[3][3] - VP[3][1]);		/* top */
	p[4] = Plane(w_axis - z_axis, VP[3][3] - VP[3][2]);		/* far */
	p[5] = Plane(w_axis + z_axis, VP[3][3] + VP[3][2]);		/* near */
}

bool Frustum::isInside(SceneNode & node) {
	for (int i = 0; i < 6; i++)
		if (!p[i].sphereInPlane(node.pos(), node.radius())) return false;
	return true;
}
}