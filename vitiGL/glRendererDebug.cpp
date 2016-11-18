#include "glRendererDebug.hpp"

#include <iostream>

namespace vitiGL {

glRendererDebug * glRendererDebug::instance() {
	static glRendererDebug instance;
	return &instance;
}

void glRendererDebug::addPoint(const glm::vec3 & pos, float radius, const glm::vec4 & color) {
	_points.push_back(glm::vec4{ pos, radius });
	_points.push_back(glm::vec4{ color });
}

void glRendererDebug::addLine(const glm::vec3 & start, const glm::vec3 & end, const glm::vec4 color) {
	_lines.push_back(glm::vec4{ start, 1.0f });
	_lines.push_back(color);
	_lines.push_back(glm::vec4{ end, 1.0f });
	_lines.push_back(color);
}

void glRendererDebug::addThickLine(const glm::vec3 & start, const glm::vec3 & end, float width, glm::vec4 color) {
	_tLines.push_back(glm::vec4{ start, width });
	_tLines.push_back(color);
	_tLines.push_back(glm::vec4{ end, width });
	_tLines.push_back(color);
}

void glRendererDebug::draw(const Camera & cam) {
	/* get the size in bytes for all data:*/
	size_t size = _points.size() + _lines.size() + _tLines.size();
	size *= 4 * sizeof(float);

	/* get the offsets: */
	size_t offsetLines = _points.size();
	size_t offsetTLines = offsetLines + _lines.size();

	size_t stride = 2 * 4 * sizeof(float);

	/* configure OpenGL-Buffers: */
	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);

	/* buffer data: */
	if (!_points.empty()) glBufferSubData(
		GL_ARRAY_BUFFER, 
		0, 
		4 * _points.size() * sizeof(float), 
		_points.data());
	if (!_lines.empty()) glBufferSubData(
		GL_ARRAY_BUFFER, 
		4 * offsetLines * sizeof(float), 
		4 * _lines.size() * sizeof(float), 
		_lines.data());
	if (!_tLines.empty()) glBufferSubData(
		GL_ARRAY_BUFFER, 
		4 * offsetTLines * sizeof(float), 
		4 * _tLines.size() * sizeof(float), 
		_tLines.data());

	/* vertex attrib pointer: */
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, stride, nullptr);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(4 * sizeof(float)));
	glEnableVertexAttribArray(1);

	/* draw: */
	if (!_points.empty()) {
		_pShader.on();
		cam.setVPUniform(_pShader);
		glUniform1f(_pShader.getUniform("aspect"), 1.0f / cam.aspect());
		glDrawArrays(GL_POINTS, 0, _points.size() >> 1);
		_pShader.off();
	}
	if (!_lines.empty()) {
		_lShader.on();
		cam.setVPUniform(_lShader);
		glUniform1f(_lShader.getUniform("aspect"), 1.0f / cam.aspect());
		glDrawArrays(GL_LINE, offsetLines >> 1, _lines.size() >> 1);
		_lShader.off();
	}
	if (!_tLines.empty()) {
		_tLShader.on();
		cam.setVPUniform(_tLShader);
		glUniform1f(_tLShader.getUniform("aspect"), 1.0f / cam.aspect());
		glDrawArrays(GL_LINE, offsetTLines >> 1, _tLines.size() >> 1);
		_tLShader.off();
	}

	/* clear all vectors: */
	_points.clear();
	_lines.clear();
	_tLines.clear();
}

glRendererDebug::glRendererDebug()
	:	_pShader{ "Shaders/Debug/vertex.glsl", "Shaders/Debug/fragment.glsl", "Shaders/Debug/points.geo.glsl" },
		_tLShader{ "Shaders/Debug/vertex.glsl", "Shaders/Debug/fragment.glsl", "Shaders/Debug/lines.geo.glsl" }
{
	std::cout << "Debug Renderer init\n";

	glGenVertexArrays(1, &_vao);
	glGenBuffers(1, &_vbo);
}

}