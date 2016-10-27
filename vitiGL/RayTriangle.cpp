#include "RayTriangle.hpp"

#include "Mesh.hpp"
#include <Ray.hpp>

namespace vitiGL {

Shader RayTriangle::_shader;
GLuint RayTriangle::_buffer;

RayTriangle::RayTriangle() {
}

void RayTriangle::start() {
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
	glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, 100000 * size, nullptr, GL_DYNAMIC_COPY);
}

bool RayTriangle::update(const Mesh * mesh, const vitiGEO::Ray * ray, std::vector<glm::vec3>& output) {
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

	/* unfortunatly, the feedback buffer is filled with ray-triangle intersection data
	   for every triangle in the mesh, so we need this ugly hack to filter out all
	   data from triangles which do not intersect: */

	std::vector<float> out(3 * mesh->countVertices());
	std::vector<float> hits;

	glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, out.size() * sizeof(float), out.data());

	for (int i = 0; i < out.size(); i++) {
		if (out[i] != 0) hits.push_back(out[i]);
	}

	/* another ugly hack because every hit gets registered three times: */
	for (int i = 0; i < hits.size(); i += 9) {
		glm::vec3 temp = { hits[i + 2], hits[i], hits[i + 1] };
		output.push_back(temp);
	}

	return false;
}
}