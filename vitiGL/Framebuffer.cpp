#include "Framebuffer.hpp"
#include "Error.hpp"
#include "vitiGlobals.hpp"

#include <iostream>
#include <glm/gtc/type_ptr.hpp>

namespace vitiGL {

Framebuffer::Framebuffer(int width, int height, 
						 const std::string& vertexShader, const std::string& fragmentShader,
						 bool setUniforms)
	:	_fbo	{ 0 },
		_tbo	{ 0 },
		_width	{ width },
		_height	{ height },
		_shader	{ vertexShader, fragmentShader}
{
#ifdef CONSOLE_LOG
	std::cout << "Framebuffer initializing...";
#endif

	init();

	//give an empty kernel at start:
	glm::mat3 kernel{};
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			kernel[i][j] = 0.0f;
	kernel[1][1] = 1.0f;

	if (setUniforms) {
		_shader.on();
		glUniformMatrix3fv(_shader.getUniform("kernel"), 1, GL_FALSE, glm::value_ptr(kernel));
		glUniform1f(_shader.getUniform("width"), float(width));
		glUniform1f(_shader.getUniform("height"), float(height));
		_shader.off();
	}

#ifdef CONSOLE_LOG
	std::cout << "\t\tfinished\n";
#endif
}

Framebuffer::~Framebuffer() {
	if (_fbo) glDeleteFramebuffers(1, &_fbo);
}

void Framebuffer::on(bool clear) {
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
	glViewport(0, 0, _width, _height);
	glEnable(GL_DEPTH_TEST);
	if (clear) glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	else glClear(GL_DEPTH_BUFFER_BIT);
}

void Framebuffer::off() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, globals::window_w, globals::window_h);
}

void Framebuffer::draw() {
	_quad.draw(_shader, _tbo);
}

void Framebuffer::setKernel(Kernel kernel) {
	glm::mat3 K;
	switch (kernel) {
	case Kernel::none:
		K[0][0] = 0.0f;
		K[0][1] = 0.0f;
		K[0][2] = 0.0f;
		K[1][0] = 0.0f;
		K[1][1] = 0.0f;
		K[1][2] = 0.0f;
		K[2][0] = 0.0f;
		K[2][1] = 0.0f;
		K[2][0] = 0.0f;
		break;
	case Kernel::blur:
		K[0][0] = 1.0f;
		K[0][1] = 1.0f;
		K[0][2] = 1.0f;
		K[1][0] = 1.0f;
		K[1][1] = 1.0f;
		K[1][2] = 1.0f;
		K[2][0] = 1.0f;
		K[2][1] = 1.0f;
		K[2][0] = 1.0f;
		K /= 9.0f;
		break;
	case Kernel::sharpen:
		K[0][0] = 0.0f;
		K[0][1] = -1.0f;
		K[0][2] = 0.0f;
		K[1][0] = -1.0f;
		K[1][1] = 5.0f;
		K[1][2] = -1.0f;
		K[2][0] = 0.0f;
		K[2][1] = -1.0f;
		K[2][0] = 0.0f;
		K /= 2;
		break;
	case Kernel::gaussianBlur:
		K[0][0] = 1.0f;
		K[0][1] = 1.0f;
		K[0][2] = 1.0f;
		K[1][0] = 1.0f;
		K[1][1] = 4.0f;
		K[1][2] = 1.0f;
		K[2][0] = 1.0f;
		K[2][1] = 1.0f;
		K[2][0] = 1.0f;
		K /= 12;
		break;
	case Kernel::hGradient:
		K[0][0] = -1.0f;
		K[0][1] = -1.0f;
		K[0][2] = -1.0f;
		K[1][0] = 0.0f;
		K[1][1] = 0.0f;
		K[1][2] = 0.0f;
		K[2][0] = 1.0f;
		K[2][1] = 1.0f;
		K[2][0] = 1.0f;
		break;
	case Kernel::vGradient:
		K[0][0] = -1.0f;
		K[0][1] = 0.0f;
		K[0][2] = 1.0f;
		K[1][0] = -1.0f;
		K[1][1] = 0.0f;
		K[1][2] = 1.0f;
		K[2][0] = -1.0f;
		K[2][1] = 0.0f;
		K[2][0] = 1.0f;
		break;
	case Kernel::emboss:
		K[0][0] = -2.0f;
		K[0][1] = -1.0f;
		K[0][2] = 0.0f;
		K[1][0] = -1.0f;
		K[1][1] = 1.0f;
		K[1][2] = 1.0f;
		K[2][0] = 0.0f;
		K[2][1] = 1.0f;
		K[2][0] = 2.0f;
		K /= 2;
		break;
	case Kernel::edges:
		K[0][0] = 0.0f;
		K[0][1] = 1.0f;
		K[0][2] = 0.0f;
		K[1][0] = 1.0f;
		K[1][1] = -4.0f;
		K[1][2] = 1.0f;
		K[2][0] = 0.0f;
		K[2][1] = 1.0f;
		K[2][0] = 0.0f;
		break;
	}
	_shader.on();
	glUniformMatrix3fv(_shader.getUniform("kernel"), 1, GL_FALSE, glm::value_ptr(K));
	_shader.off();
}

GLuint Framebuffer::copyTexture() const {
	/* create and configure a new texture object: */
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _width, _height, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	/* copy fbo-content to the new texture: */
	glCopyImageSubData(_tbo, GL_TEXTURE_2D, 0, 0, 0, 0, texture, GL_TEXTURE_2D, 0, 0, 0, 0, _width, _height, 1);

	return texture;
}

/* maybe a bad idea? */
void Framebuffer::setTextureSize(int w, int h) {
	glBindTexture(GL_TEXTURE_2D, _tbo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_FLOAT, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Framebuffer::init() {
	glGenFramebuffers(1, &_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
	
	//generate, bind and configure the framebuffers texture buffer:
	glGenTextures(1, &_tbo);
	glBindTexture(GL_TEXTURE_2D, _tbo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _width, _height, 0, GL_RGB, GL_FLOAT, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glBindTexture(GL_TEXTURE_2D, 0);

	//attach the texture to the framebuffer:
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _tbo, 0);

	/* we also attach a depht and stencil buffer to the framebuffer: */
	GLuint rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _width, _height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	/* attach the renderbuffer to the framebuffer: */
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);


	//check if the framebuffer is ready:
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw initError("Failed to initialite Framebuffer!", "");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
}