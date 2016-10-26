/* Class to perform Ray-Triangle-Intersection tests on the gpu */

#pragma once

#include <glm/glm.hpp>

#include <Ray.hpp>
#include "Shader.hpp"

namespace vitiGL {

class Mesh;

class RayTriangle {
public:
	RayTriangle();

	static bool update(const Mesh* mesh, const vitiGEO::Ray* ray, glm::vec3& output);

private:
	static Shader _shader;
	static GLuint _buffer;
};
}