#pragma once

/* Singleton class to draw lines and points for debug purposes */

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>

#include "Shader.hpp"
#include "Camera.hpp"

namespace vitiGL {

class glRendererDebug {
public:
	static glRendererDebug* instance();

	void addPoint(const glm::vec3& pos, float radius, const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 0.5f });
	void addLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4 color = { 1.0f, 1.0f, 1.0f, 0.5f });
	void addThickLine(const glm::vec3 &start, const glm::vec3& end, float width, glm::vec4 color = { 1.0f, 1.0f, 1.0f, 0.5f });

	void draw(const Camera& cam);

protected:
	glRendererDebug();

	std::vector<glm::vec4> _points;
	std::vector<glm::vec4> _tLines;
	std::vector<glm::vec4> _lines;
	
	Shader _pShader;
	Shader _tLShader;
	Shader _lShader;

	GLuint _vao;
	GLuint _vbo;
};

}