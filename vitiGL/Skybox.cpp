#include "Skybox.hpp"

#include "Shader.hpp"
#include <src/SOIL.h>

namespace vitiGL {

Skybox::Skybox(const std::vector<std::string>& texturePath) 
	:	IGameObject { ObjType::skybox, "" },
		_tbo		{ 0 },
		_vao		{ 0 },
		_vbo		{ 0 }
{

	/* generate texture and load the cubemap: */
	glGenTextures(1, &_tbo);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _tbo);

	int w{ 0 }; int h{ 0 };
	unsigned char* img;

	for (size_t i = 0; i < texturePath.size(); i++) {
		img = SOIL_load_image(texturePath[i].c_str(), &w, &h, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	/* initialize the cubes vertices: */
	initVertices();
}

Skybox::~Skybox() {
	if (_tbo) glDeleteTextures(1, &_tbo);
	if (_vao) glDeleteVertexArrays(1, &_vao);
	if (_vbo) glDeleteBuffers(1, &_vbo);
}

void Skybox::draw(const Shader & shader) const {
	glDepthFunc(GL_LEQUAL);

	glBindVertexArray(_vao);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(shader.getUniform("skybox"), 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _tbo);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);

	glDepthFunc(GL_LESS);
}

/* initialize and upload vertex data for the unit-cube*/
void Skybox::initVertices() {
	float vertices[] = {       
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	glGenVertexArrays(1, &_vao);
	glGenBuffers(1, &_vbo);
	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

	glBindVertexArray(0);
}
}
