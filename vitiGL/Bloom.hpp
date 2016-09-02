#pragma once

#include <GL/glew.h>

namespace vitiGL {

class Bloom {
public:
	Bloom(int width, int height);
	~Bloom();

private:
	void	initFramebuffer();

	GLuint	_fbo;
	GLuint	_tbo[2];

	int		_w;
	int		_h;
};

}
