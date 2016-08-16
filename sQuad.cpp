#include "sQuad.hpp"
#include <glm/glm.hpp>


namespace vitiGL {

sQuad::sQuad() 
	:	_vao{ 0 },
		_vbo{ 0 }
{
	initVertices();
}

sQuad::~sQuad() {
	if (_vao) glDeleteVertexArrays(1, &_vao);
	if (_vbo) glDeleteBuffers(1, &_vbo);
}

void sQuad::draw(const Shader & shader, GLuint texture, const std::string& textureUniformName) {
	glDisable(GL_DEPTH_TEST);	//because we are drawing 2D
	
	shader.on();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(shader.getUniform(textureUniformName), 0);

	glBindVertexArray(_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	shader.off();
	glEnable(GL_DEPTH_TEST);
}

void sQuad::draw(const Shader & shader, const std::vector<GLuint>& textures, const std::vector<std::string> textureNames) {
	glDisable(GL_DEPTH_TEST);
	
	shader.on();
	for (int i = 0; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glUniform1i(shader.getUniform(textureNames[i]), i);
	}

	glBindVertexArray(_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	shader.off();
	glEnable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);
}


void sQuad::initVertices() {

	float quadVertices[] = {
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};

	//create and bind the buffers:
	glGenVertexArrays(1, &_vao);
	glGenBuffers(1, &_vbo);
	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);

	//upload the buffer content:
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glBindVertexArray(0);
}

}