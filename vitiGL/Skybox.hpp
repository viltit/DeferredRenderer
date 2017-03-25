/* 
	Class to display a skybox
	inherits some superfluous methods from IGameObject in order to be attachable to the Scene
*/

#pragma once

#include <GL/glew.h>

#include <string>
#include <vector>

#include "IGameObject.hpp"

namespace vitiGL {

class Shader;

class Skybox : public IGameObject {
public:
	Skybox(const std::vector<std::string>& texturePath);
	~Skybox();

	virtual void draw(const Shader& shader) const override;

	std::vector<std::string>& textures() { return _textures; }

protected:
	void initVertices();

	GLuint _tbo;
	GLuint _vao;
	GLuint _vbo;

	std::vector<std::string> _textures; //for scene saving
};
}
