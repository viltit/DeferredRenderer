#include "Bloom.hpp"

#include "Error.hpp"

namespace vitiGL {

Bloom::Bloom(int width, int height) 
	:	_w { width },
		_h { height}
{}


Bloom::~Bloom(){
	if (_fbo) glDeleteFramebuffers(1, &_fbo);
	if (_tbo[0]) glDeleteTextures(2, _tbo);
}

void Bloom::initFramebuffer() {
	glGenFramebuffers(1, &_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

	glGenTextures(2, _tbo);

	for (size_t i = 0; i < 2; i++) {
		glBindTexture(GL_TEXTURE_2D, _tbo[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _w, _h, 0, GL_RGB, GL_FLOAT, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, _tbo[i], 0);
	}

	GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw initError("<Bloom::initFramebuffer> Failed to initialite Framebuffer!", "");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}