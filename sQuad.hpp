#pragma once

/* simple quad to use as drawing area for framebuffers */

#include <GL/glew.h>
#include "Shader.hpp"

#include <vector>
#include <string>

namespace vitiGL {

class sQuad {
public:

	/* w and h = 1 -> draw screen sized */
	sQuad		(float w = 1.0f, float h = 1.0f);
	~sQuad		();

	void draw	(const Shader& shader, GLuint texture, 
				 const std::string& textureUniformName = "tex");

	void draw	(const Shader& shader,
				const std::vector<GLuint>& textures,
				const std::vector<std::string> textureNames);

private:
	void initVertices(float w, float h);

	GLuint _vao;
	GLuint _vbo;

	float _w;
	float _h;
};
}
