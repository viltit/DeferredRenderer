#pragma once

/* Singleton class to draw lines and points for debug purposes */

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>

#include "Shader.hpp"
#include "Camera.hpp"

#include <bt/btBulletDynamicsCommon.h>

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

class glRendererBTDebug : public btIDebugDraw {
public:
	static glRendererBTDebug* instance();

	void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
	void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override {  };

	void setDebugMode(int mode) override { _mode = mode; }
	int getDebugMode() const override { return _mode; }
	void	reportErrorWarning(const char* warningString) {}

	void	draw3dText(const btVector3& location, const char* textString) {}


private:
	glRendererBTDebug() { _mode = DBG_DrawWireframe | DBG_DrawAabb; };
	
	int _mode;
};

}