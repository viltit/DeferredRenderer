#include "Material.hpp"

#include "Shader.hpp"
#include "Error.hpp"

#include <assert.h>

namespace vitiGL {

Material::Material() 
	: _shiny	{ 22.0f },
	  _transp	{ 0.0f }
{
	_tbo.resize(3);
}

Material::Material(std::vector<GLuint> textures, float shiny, float transparency)
	:	_tbo		{ textures },
		_shiny		{ shiny },
		_transp		{ transparency }
{
	_tbo.resize(3);
}


Material::~Material() {
}

void Material::setUniforms(const Shader & shader) const {

	assert(_tbo.size() < 4 , "<Material>: more than 4 textures defined");

	for (size_t i = 0; i < _tbo.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, _tbo[i]);
		glUniform1i(shader.getUniform("material." + texNames[i]), i);
	}

	//set subroutine for normal calculation:
	//TO DO: SORT OBJECTS IN RENDERER OR SCENE TO AVOID SETTING UNIFORM FOR EACH OBJECT

	if (_tbo[TEXTURE_NORMAL] == 0) {
		GLuint normalCalc = glGetSubroutineIndex(shader.program(), GL_FRAGMENT_SHADER, "getNormalFromVertex");
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &normalCalc);
	}
	else {
		GLuint normalCalc = glGetSubroutineIndex(shader.program(), GL_FRAGMENT_SHADER, "getNormalFromTexture");
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &normalCalc);
	}
}

void Material::setTexture(int type, GLuint texture) {
	switch (type) {
	case TEXTURE_DIFFUSE:
		if (_tbo.size() < 1) _tbo.resize(1);
		_tbo[0] = texture;
		break;
	case TEXTURE_SPECULAR:
		if (_tbo.size() < 2) _tbo.resize(2);
		_tbo[1] = texture;
		break;
	case TEXTURE_NORMAL:
		if (_tbo.size() < 3) _tbo.resize(3);
		_tbo[2] = texture;
		break;
	default:
		throw vitiError("<Material::setTexture>: invalid type index.");
	}
}

Material & Material::operator=(const Material mat) {
	if (this != &mat) {
		_tbo = mat._tbo;
		_shiny = mat._shiny;
		_transp = mat._transp;
	}

	return *this;
}

}
