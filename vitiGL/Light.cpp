#include "Light.hpp"
#include "vitiGL.hpp"

namespace vitiGL {

dLight::dLight(const std::string& uniformName, const glm::vec3& dir, 
			   const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular)
	:	_uniform	{ uniformName },
		_dir		{ glm::normalize(dir) },
		_ambient	{ ambient },
		_diffuse	{ diffuse },
		_specular	{ specular }
{}

dLight::~dLight() {
}

void dLight::setProperty(lightProps property, const glm::vec3 & value, const Shader& shader) {
	shader.on();
	switch (property) {
	case lightProps::dir:
		_dir = glm::normalize(value);
		glUniform3f(shader.getUniform(_uniform + ".dir"), _dir.x, _dir.y, _dir.z);
		//glUniform3f(shader.getUniform(_uniform + "Dir"), _dir.x, _dir.y, _dir.z); //wip
		break;
	case lightProps::ambient:
		_ambient = value;
		//glUniform3f(shader.getUniform(_uniform + ".ambient"), _ambient.r, _ambient.g, _ambient.b);
		break;
	case lightProps::diffuse:
		glUniform3f(shader.getUniform(_uniform + ".diffuse"), _diffuse.r, _diffuse.g, _diffuse.b);
		break;
	case lightProps::specular:
		glUniform3f(shader.getUniform(_uniform + ".specular"), _specular.r, _specular.g, _specular.b);
		break;
	default:
#ifdef CONSOLE_LOG
		std::cout << "<dLight::setProperty> Trying to set inexistant property.\n";
#endif
		break;
	}
	shader.off();
}

void dLight::setUniforms(const Shader& shader) {
	shader.on();
	glUniform3f(shader.getUniform(_uniform + ".dir"), _dir.x, _dir.y, _dir.z);
	//glUniform3f(shader.getUniform(_uniform + "Dir"), _dir.x, _dir.y, _dir.z); //wip
	//glUniform3f(shader.getUniform(_uniform + ".ambient"), _ambient.r, _ambient.g, _ambient.b);
	glUniform3f(shader.getUniform(_uniform + ".diffuse"), _diffuse.r, _diffuse.g, _diffuse.b);
	glUniform3f(shader.getUniform(_uniform + ".specular"), _specular.r, _specular.g, _specular.b);
	shader.off();
}

void dLight::draw(const Shader & shader) {
	_quad.drawNaked(shader);
}

pLight::pLight	(const std::string & uniformName, const glm::vec3 & attenuation, 
				const glm::vec3 & pos, const glm::vec3 & ambient, const glm::vec3 & diffuse, const glm::vec3 & specular)
	:	_uniform		{ uniformName },
		_attenuation	{ attenuation },
		_pos			{ pos },
		_ambient		{ ambient },
		_diffuse		{ diffuse },
		_specular		{ specular }
{}

void pLight::setProperty(lightProps property, const glm::vec3 & value, const Shader & shader) {
	shader.on();
	switch (property) {
	case lightProps::pos:
		_pos = value;
		glUniform3f(shader.getUniform(_uniform + ".pos"), _pos.x, _pos.y, _pos.z);
		glUniform3f(shader.getUniform(_uniform + "Pos"), _pos.x, _pos.y, _pos.z);
		break;
	case lightProps::ambient:
		_ambient = value;
		glUniform3f(shader.getUniform(_uniform + ".ambient"), _ambient.r, _ambient.g, _ambient.b);
		break;
	case lightProps::diffuse:
		glUniform3f(shader.getUniform(_uniform + ".diffuse"), _diffuse.r, _diffuse.g, _diffuse.b);
		break;
	case lightProps::specular:
		glUniform3f(shader.getUniform(_uniform + ".specular"), _specular.r, _specular.g, _specular.b);
		break;
	case lightProps::attenuation:
		glUniform3f(shader.getUniform(_uniform + ".attenuation"), _attenuation.x, _attenuation.y, _attenuation.z);
		break;
	default:
#ifdef CONSOLE_LOG
		std::cout << "<pLight::setProperty> Trying to set inexistant property.\n";
#endif
		break;
	}
	shader.off();
}

void pLight::setUniforms(const Shader & shader) {
	shader.on();
	glUniform3f(shader.getUniform(_uniform + ".pos"), _pos.x, _pos.y, _pos.z);
	glUniform3f(shader.getUniform(_uniform + "Pos"), _pos.x, _pos.y, _pos.z);
	glUniform3f(shader.getUniform(_uniform + ".ambient"), _ambient.r, _ambient.g, _ambient.b);
	glUniform3f(shader.getUniform(_uniform + ".diffuse"), _diffuse.r, _diffuse.g, _diffuse.b);
	glUniform3f(shader.getUniform(_uniform + ".specular"), _specular.r, _specular.g, _specular.b);
	glUniform3f(shader.getUniform(_uniform + ".attenuation"), _attenuation.x, _attenuation.y, _attenuation.z);
	shader.off();
}

void pLight::draw(const Shader & shader)
{
}

}