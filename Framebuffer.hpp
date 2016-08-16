#pragma once

#include <GL/glew.h>
#include <string>
#include "Shader.hpp"
#include "sQuad.hpp"

namespace vitiGL {

enum class Kernel {
	none,
	blur,
	sharpen,
	gaussianBlur,
	hGradient,
	vGradient,
	emboss,
	edges
};

class Framebuffer {
public:
	Framebuffer	(int width, int height, 
				const std::string& vertexShader = "Shaders/framebuffer.vert.glsl", 
				const std::string& fragmentShader = "Shaders/framebuffer.frag.glsl");
	~Framebuffer();

	void	on();
	void	off();
	void	draw();

	void	setKernel(Kernel kernel);

	GLuint	texture() const	{ return _tbo; }

	void	setTextureSize(int w, int h);

protected:
	virtual void init();

	Shader	_shader;
	
	sQuad	_quad;

	GLuint	_fbo;
	GLuint	_tbo;
	int		_width;
	int		_height;
};
}
