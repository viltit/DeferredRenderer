#pragma once

/* simple quad to use as drawing area for framebuffers */

#include <GL/glew.h>
#include "Shader.hpp"

#include <vector>
#include <string>

namespace vitiGL {

class sQuad {
public:
	sQuad();
	~sQuad();

	void draw	(const Shader& shader, GLuint texture, 
				 const std::string& textureUniformName = "tex");

	void draw	(const Shader& shader,
				const std::vector<GLuint>& textures,
				const std::vector<std::string> textureNames);

private:
	void initVertices();

	GLuint _vao;
	GLuint _vbo;
};
}
