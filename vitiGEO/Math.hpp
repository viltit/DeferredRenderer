/*	This header files provides some additional functions to 
	glm-matrices and vectors, which glm itself does not provide */

#pragma once

#include <glm/glm.hpp>

namespace vitiGEO {

/* get the translation vector from a matrix: */
glm::vec3 getTranslation(const glm::mat4& M);

/* put translation vector into matrix: */
glm::mat4 setTranslation(const glm::mat4& M, const glm::vec3& pos);

}