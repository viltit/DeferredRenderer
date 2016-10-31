#include "RayTriangle.hpp"

#include "Mesh.hpp"
#include <Ray.hpp>

namespace vitiGL {

Shader RayTriangle::_shader;
GLuint RayTriangle::_buffer;

RayTriangle::RayTriangle() {
}

void RayTriangle::start() {
	/* determine the size of the feedback buffer */
	float size = 3 * 3 * 3 * sizeof(float);

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
	glGenBuffers(1, &_buffer);
	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, _buffer);
	glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, size, nullptr, GL_DYNAMIC_COPY);
}

bool RayTriangle::update(const Mesh * mesh, const vitiGEO::Ray * ray, std::vector<glm::vec3>& output) {

	std::vector<float> out(27);
	std::vector<float> hits;

	_shader.on();
	glEnable(GL_RASTERIZER_DISCARD);

	glUniform3f(_shader.getUniform("ray.origin"), ray->_origin.x, ray->_origin.y, ray->_origin.z);
	glUniform3f(_shader.getUniform("ray.direction"), ray->_delta.x, ray->_delta.y, ray->_delta.z);

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, _buffer);

	glClearBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, GL_R32F, GL_RGBA, GL_FLOAT, nullptr);

	glBeginTransformFeedback(GL_TRIANGLES);
	mesh->drawNaked(_shader);
	glEndTransformFeedback();

	glDisable(GL_RASTERIZER_DISCARD);
	_shader.off();

	glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, out.size() * sizeof(float), out.data());

	/* discard 0-values: */
	for (int i = 0; i < out.size(); i++) {
		if (out[i] != 0) hits.push_back(out[i]);
	}

	/* an ugly hack because every hit gets registered three times: */
	for (int i = 0; i < hits.size(); i += 9) {
		glm::vec3 temp = { hits[i + 2], hits[i], hits[i + 1] };
		output.push_back(temp);
	}

	return true;
}
}