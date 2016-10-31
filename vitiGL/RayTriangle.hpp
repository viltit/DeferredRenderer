/*	Class to perform Ray-Triangle-Intersection tests on the gpu  */

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

	/*	output will store the first three hit points as vec3. the vec3 contains the following data: 
			.x => the factor t of the rays intersection point: intersection = ray.origin + ray.delta * t / |ray.delta| 
			.y and .z : the uv coordinates of the triangles hit point								*/
	static bool update(const Mesh* mesh, const vitiGEO::Ray* ray, std::vector<glm::vec3>& output);

private:
	static Shader _shader;
	static GLuint _buffer;
};
}