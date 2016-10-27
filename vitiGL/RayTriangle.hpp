/*	Class to perform Ray-Triangle-Intersection tests on the gpu 

	TO DO: Fix the hacks mentioned in the cpp with a robust solution 
	(it works now, but in a potentially dangerous way) */

#pragma once

#include <glm/glm.hpp>

#include <Ray.hpp>
#include "Shader.hpp"

namespace vitiGL {

class Mesh;

class RayTriangle {
public:
	RayTriangle();

	static void start();
	static bool update(const Mesh* mesh, const vitiGEO::Ray* ray, std::vector<glm::vec3>& output);

private:
	static Shader _shader;
	static GLuint _buffer;
};
}