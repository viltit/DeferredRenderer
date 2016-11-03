#include "Math.hpp"

namespace vitiGEO {

/* get the translation vector from a matrix: */
glm::vec3 getTranslation(const glm::mat4& M) {
	return glm::vec3{ M[3][0], M[3][1], M[3][2] };
}

/* put translation vector into matrix: */
glm::mat4 setTranslation(const glm::mat4& M, const glm::vec3& pos) {
	glm::mat4 M2{ M };
	M2[3][0] = pos.x; M2[3][1] = pos.y; M2[3][2] = pos.z;
	return M2;
}

}