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
	GaussBlur(int width = globals::window_w, int height = globals::window_h);
	~GaussBlur();

	GLuint blur(GLuint texture, int iterations = 5);

	/* first downsample with Framebuffer Blitting, then blur: */
	GLuint blurDS(GLuint framebuffer, const glm::ivec2& originSize, 
				  int iterations = 5, int colorAttachment = 0);

private:
	void initFramebuffer();

	GLuint	_fbo[3];	//[0, 1]: ping-pong-framebuffer; [2]: For downsampling
	GLuint	_tbo[3];	//and the corresponding texture attachments

	Shader	_shader;

	int		_w;
	int		_h;

	sQuad	_quad;
};
}
