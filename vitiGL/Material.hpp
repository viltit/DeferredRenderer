/* Class for holding material properties like diffuse or specular textures */

#pragma once

#include <vector>
#include <string>

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
	Material(std::vector<GLuint> textures, float shiny = 22.0f, bool transparency = false);

	~Material();

	Material(const Material& mat) = delete;
	Material(Material&& mat) = delete;

	//shader has to be on before calling the function
	void setUniforms(const Shader& shader) const;

	void setTexture(int type, GLuint texture);

	/* getters and setters: */
	void setShiny(float val)	{ _shiny = val; }
	float shiny() const			{ return _shiny; }

	bool isTransparent() const  { return (_tbo.size() > 3 && _tbo[3] != 0);  }

	Material& operator = (const Material& mat);

private:
	std::vector<GLuint> _tbo;
	float _shiny;

	bool _transparent;

	const std::string texNames[5] = {
		"diffuse",
		"specular",
		"normal",
		"alpha",
		"bump"
	};
};
}
