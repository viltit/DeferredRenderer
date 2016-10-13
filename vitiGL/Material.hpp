/* Class for holding material properties like diffuse or specular textures */

#pragma once

#include <vector>

#include <GL/glew.h>

#define TEXTURE_DIFFUSE 0
#define TEXTURE_SPECULAR 1
#define TEXTURE_NORMAL 2
#define TEXTURE_BUMP 3
#define TEXTURE_ALPHA 4

namespace vitiGL {

class Shader;

class Material {
public:
	Material();

	//textures HAVE to be in order: [0] -> diffuse; [1] -> specular; [2] -> normal; [3] -> bump
	Material(std::vector<GLuint> textures, float shiny = 22.0f, float transparency = 0.0f);

	~Material();

	//shader has to be on before calling the function
	void setUniforms(const Shader& shader) const;

	void setTexture(int type, GLuint texture);

	/* getters and setters: */
	void setShiny(float val)	{ _shiny = val; }
	float shiny() const			{ return _shiny; }

	void setTransparency(float val) { _transp = val; }
	float transparency() const		{ return _transp; }

	Material& operator = (const Material mat);

private:
	std::vector<GLuint> _tbo;
	float _shiny;
	float _transp;

	const std::string texNames[5] = {
		"diffuse",
		"specular",
		"normal",
		"bump",
		"alpha"
	};
};
}
