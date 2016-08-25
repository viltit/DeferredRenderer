#pragma once

/* simple quad to use as drawing area for framebuffers */

#include <GL/glew.h>
#include "Shader.hpp"

#include <vector>
#include <string>

namespace vitiGL {

enum class QuadPos {
	fullscreen,
	topRight,
	aboveMiddleRight,
	belowMiddleRight
	//to be continued...
};

class sQuad {
public:

	/* w and h = 1 -> draw screen sized */
	sQuad		(QuadPos pos = QuadPos::fullscreen);
	~sQuad		();

	void draw	(const Shader& shader, GLuint texture, 
				 const std::string& textureUniformName = "tex");

	void draw	(const Shader& shader,
				const std::vector<GLuint>& textures,
				const std::vector<std::string> textureNames);

	/* shader needs to be on for this one! (as opposed to the above functions) */
	void drawNaked(const Shader& shader);

private:
	void initVertices();

	GLuint	_vao;
	GLuint	_vbo;

	QuadPos	_pos;
};
}
