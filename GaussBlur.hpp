#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "vitiGlobals.hpp"
#include "sQuad.hpp"
#include "Shader.hpp"

/*	class to blur a texture with a two pass gaussian blur (ie., first we blur horizontal, then we blur vertical)  */

namespace vitiGL {

class GaussBlur {
public:
	GaussBlur(int iterations = 5, int width = globals::window_w, int height = globals::window_h);
	~GaussBlur();

	GLuint blur(GLuint texture);

private:
	void initFramebuffer();

	GLuint	_fbo[2];	//ping-pong-framebuffer
	GLuint	_tbo[2];	//and the corresponding texture attachments

	Shader	_shader;

	int		_w;
	int		_h;
	int		_iterations; //how many times we blur(1*horizontal and 1*vertical -> amount = 1);

	sQuad	_quad;
};
}
