#pragma once

#include <glm/glm.hpp>
#include <string>
#include "Shader.hpp"
#include "Shape.hpp"

namespace vitiGL {

enum class lightProps {
	dir,
	pos,
	ambient,
	diffuse,
	specular,
	attenuation
};

/* Base class for all lights: --------------------------------------------------------------------- */
class Light : public SceneObject {
public:
	Light() {}
	virtual ~Light() {}

	virtual void setProperty(lightProps property, const glm::vec3& value, const Shader& shader) = 0;
	virtual void setUniforms(const Shader& shader) = 0;

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

	/* getters and setters: */
	glm::vec3 dir() const	{ return _dir; }

private:
	std::string		_uniform;
	glm::vec3		_dir;
	glm::vec3		_ambient;
	glm::vec3		_diffuse;
	glm::vec3		_specular;
};

/* Point Light ----------------------------------------------------------------------------------- */
class pLight : public Light {
public:
	pLight	(const std::string& uniformName,
			 const glm::vec3& attenuation = { 1.0f, 0.07f, 0.017f },
			 const glm::vec3& pos =		{ 0.0f, 1.0f, 0.0f }, 
			 const glm::vec3& ambient = { 0.1f, 0.1f, 0.1f },
			 const glm::vec3& diffuse = { 0.6f, 0.6f, 0.6f }, 
			 const glm::vec3& specular = { 1.0f, 1.0f, 1.0f });

	void setProperty(lightProps property, const glm::vec3& value, const Shader& shader);
	void setUniforms(const Shader& shader);

private:
	std::string		_uniform;
	glm::vec3		_pos;
	glm::vec3		_attenuation;
	glm::vec3		_ambient;
	glm::vec3		_diffuse;
	glm::vec3		_specular;
};

}
