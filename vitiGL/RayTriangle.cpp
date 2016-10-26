#include "RayTriangle.hpp"

#include "Mesh.hpp"
#include <Ray.hpp>

namespace vitiGL {

Shader RayTriangle::_shader;
GLuint RayTriangle::_buffer;

RayTriangle::RayTriangle() {

	/* Set up the shader program: */
	_shader.init("Shaders/Raytracing/Triangle.vert.glsl", 
				 "Shaders/Raytracing/Triangle.frag.glsl", 
				 "Shaders/Raytracing/Triangle.geo.glsl");

	const char* varyingNames[] = {
		"uv",
		"t"
	};
	glTransformFeedbackVaryings(_shader.program(), 2, varyingNames, GL_INTERLEAVED_ATTRIBS);

	_shader.link();

	/* set up the (mini)buffer for the shaders output: */
	int size = sizeof(glm::vec2) + sizeof(float);

	glGenBuffers(1, &_buffer);
	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, _buffer);
	glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, size, nullptr, GL_DYNAMIC_COPY);
}

bool RayTriangle::update(const Mesh * mesh, const vitiGEO::Ray * ray, glm::vec3 & output) {
	_shader.on();
	glEnable(GL_RASTERIZER_DISCARD);

	glUniform3f(_shader.getUniform("ray.origin"), ray->_origin.x, ray->_origin.y, ray->_origin.z);
	glUniform3f(_shader.getUniform("ray.direction"), ray->_delta.x, ray->_delta.y, ray->_delta.z);

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, _buffer);

	glBeginTransformFeedback(GL_TRIANGLES);
	mesh->drawNaked(_shader);
	glEndTransformFeedback();

	glDisable(GL_RASTERIZER_DISCARD);
	_shader.off();

	return false;
}
}