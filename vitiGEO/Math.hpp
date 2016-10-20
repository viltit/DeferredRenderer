/*	This header files provides some additional functions to 
	glm-matrices and vectors, which glm itself does not provide */

#pragma once

#include <glm/glm.hpp>

namespace vitiGEO {

/* get the translation vector from a matrix: */
glm::vec3 getTranslation(const glm::mat4& M) {
	return glm::vec3{ M[3][0], M[3][1], M[3][2] };
}


}