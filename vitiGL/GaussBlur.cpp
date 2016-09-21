#include "GaussBlur.hpp"
#include "Error.hpp"

namespace vitiGL {

GaussBlur::GaussBlur(int w, int h)
	:	_shader	{ "Shaders/gaussianBlur.vert.glsl", "Shaders/gaussianBlur.frag.glsl" },
		_w		{ w },
		_h		{ h },
		_quad	{  }
{
	initFramebuffer();
}


GaussBlur::~GaussBlur() {
	if (_fbo[0] != 0) {
		glDeleteFramebuffers(2, _fbo);
	}
	if (_tbo[0] != 0) {
		glDeleteTextures(2, _tbo);
	}
}

GLuint GaussBlur::blur(GLuint texture, int iterations) {
	glViewport(0, 0, _w, _h);

	bool horizontal = true;
	bool firstIteration = true;

	for (int i = 0; i < iterations * 2; i++) {
		_shader.on();		//a lot of on and offs, need to modify quad.draw() function to avoid
		glBindFramebuffer(GL_FRAMEBUFFER, _fbo[horizontal]);
		glUniform1i(_shader.getUniform("horizontal"), horizontal);

		_quad.draw(_shader, firstIteration ? texture : _tbo[!horizontal], "image");

		horizontal = !horizontal;
		if (firstIteration) firstIteration = false;

		_shader.off();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, globals::window_w, globals::window_h);

	return _tbo[!horizontal];
}


GLuint GaussBlur::blurDS(GLuint framebuffer, const glm::ivec2& originSize, int iterations, int colorAttachment) {
	/* we downsample to the size of the GaussBlur Framebuffers: */
	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
	glReadBuffer(GL_COLOR_ATTACHMENT0 + colorAttachment);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo[2]);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glBlitFramebuffer(0, 0, originSize.x, originSize.y, 0, 0, _w, _h, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	return blur(_tbo[2], iterations);
}


void GaussBlur::initFramebuffer() {
	glGenFramebuffers(3, _fbo);
	glGenTextures(3, _tbo);

	for (int i = 0; i < 3; i++) {
		glBindFramebuffer(GL_FRAMEBUFFER, _fbo[i]);
		glBindTexture(GL_TEXTURE_2D, _tbo[i]);
		
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB16F, _w, _h);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _tbo[i], 0);

		/* check if the framebuffer is ready to use: */
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			throw initError("<Shadowmap::Shadomap>\tCould not initialize Framebuffer.");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


}