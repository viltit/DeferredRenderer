/*	CLASS LIGHT --------------------------------------------------------------------------- 
	
	Task:	Hold all relevant information and methods for point lights and dir lights. 
			Also give a draw function for deferred rendering.

	To Do:	Add rim lights?
-------------------------------------------------------------------------------------------- */

#pragma once

#include <glm/glm.hpp>
#include <string>
#include "Shader.hpp"
#include "Shape.hpp"
#include "sQuad.hpp"

namespace vitiGL {


enum class lightProps {
	dir,
	pos,
	ambient,
	diffuse,
	specular,
	attenuation
};

class Light  {
public:
	Light() {}
	virtual ~Light() {}

	virtual void setProperty(lightProps property, const glm::vec3& value, const Shader& shader) = 0;
	virtual void setUniforms(const Shader& shader) = 0;
	virtual void draw(const Shader& shader) = 0; //for deferred rendering

protected:
};

/* Directional light ------------------------------------------------------------------------------ */
class dLight : public Light {
public:
	dLight	(const std::string& uniformName,
			 const glm::vec3& dir = { 0.0f, -1.0f, 0.0f }, 
			 const glm::vec3& ambient = { 0.1f, 0.1f, 0.1f },
			 const glm::vec3& diffuse = { 0.6f, 0.6f, 0.6f }, 
			 const glm::vec3& specular = { 1.0f, 1.0f, 1.0f });
	~dLight();

	void setProperty(lightProps property, const glm::vec3& value, const Shader& shader);
	void setUniforms(const Shader& shader);
	void draw(const Shader& shader);

	/* getters and setters: */
	glm::vec3 dir() const	{ return _dir; }

private:
	std::string		_uniform;
	glm::vec3		_dir;
	glm::vec3		_ambient;
	glm::vec3		_diffuse;
	glm::vec3		_specular;
	sQuad			_quad;
};

/* Point Light ----------------------------------------------------------------------------------- */
class pLight : public Light {
public:
	pLight	(const std::string& uniformName,
			 const glm::vec3& pos =		{ 0.0f, 1.0f, 0.0f }, 
			 const glm::vec3& ambient = { 0.1f, 0.1f, 0.1f },
			 const glm::vec3& diffuse = { 0.6f, 0.6f, 0.6f }, 
			 const glm::vec3& specular = { 1.0f, 1.0f, 1.0f },
			 const glm::vec3& attenuation = { 1.0f, 0.7f, 1.8f });

	//~pLight();

	void setProperty(lightProps property, const glm::vec3& value, const Shader& shader);
	void setUniforms(const Shader& shader);
	void draw(const Shader& shader);

private:
	/* calculate the lights radius based on the attenuation constants: */
	void calcRadius();

	std::string		_uniform;
	glm::vec3		_pos;
	glm::vec3		_attenuation;
	glm::vec3		_ambient;
	glm::vec3		_diffuse;
	glm::vec3		_specular;

	Sphere*			_sphere;
	float			_r;
};

}
