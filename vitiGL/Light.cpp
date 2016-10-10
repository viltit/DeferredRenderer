#include "Light.hpp"
#include "vitiGL.hpp"
#include "vitiGlobals.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace vitiGL {

namespace Attenuation {
	glm::vec3 r7 = { 1.0f, 0.7f, 1.8f };
	glm::vec3 r20 = { 1.0f, 0.22f, 0.2f };
	glm::vec3 r50 = { 1.0f, 0.09f, 0.032f };
	glm::vec3 r100 = { 1.0f, 0.0045f, 0.0075f };
	glm::vec3 r160 = { 1.0f, 0.027f, 0.0028f };
	glm::vec3 r200 = { 1.0f, 0.022f, 0.0019 };
}

dLight::dLight(const std::string& uniformName, const glm::vec3& dir, 
			   const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular)
	:	Light		{ ObjType::dlight },
		_uniform	{ uniformName },
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

void dLight::setUniforms(const Shader& shader) const {
	glUniform3f(shader.getUniform(_uniform + ".dir"), _dir.x, _dir.y, _dir.z);
	glUniform3f(shader.getUniform(_uniform + ".diffuse"), _diffuse.r, _diffuse.g, _diffuse.b);
	glUniform3f(shader.getUniform(_uniform + ".specular"), _specular.r, _specular.g, _specular.b);
}

void dLight::draw(const Shader & shader) const {
	_quad.drawNaked(shader);
}

/* ----------------------------------------------------------------------------------------------------------- */

pLight::pLight	
(
	Camera* camera,
	const glm::vec3 & pos, 
	const glm::vec3 & ambient, 
	const glm::vec3 & diffuse, 
	const glm::vec3 & specular, 
	const glm::vec3 & attenuation,
	const std::string & uniformName
)
	:	Light			{ ObjType::plight },
		_cam			{ camera },
		_attenuation	{ attenuation },
		_pos			{ pos },
		_ambient		{ ambient },
		_diffuse		{ diffuse },
		_specular		{ specular },
		_uniform		{ uniformName },
		_sphere			{ new Sphere{"xml/sphere.xml" } }
{
	_attenuation = Attenuation::r100;

	calcRadius(); //the math here is correct

	/* set the lights sphere original model matrix: */
	_M = glm::translate(_M, pos);
	_M = glm::scale(_M, glm::vec3{ _r,  _r,  _r });

	_sphere->setModelMatrix(_M);
}

/*
pLight::~pLight() {
	if (_sphere != nullptr) {
		delete _sphere;
		_sphere = nullptr;
	}
}*/

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

void pLight::setProperty(lightProps property, const glm::vec3 & value) {
	switch (property) {
	case lightProps::pos:
		_pos = value;
		_M[3][0] = _pos.x; _M[3][1] = _pos.y; _M[3][2] = _pos.z;
		_sphere->setModelMatrix(_M);
		break;
	case lightProps::ambient:
		_ambient = value;
		break;
	case lightProps::diffuse:
		_diffuse = value;
		break;
	case lightProps::specular:
		_specular = value;
		break;
	case lightProps::attenuation:
		_attenuation = value;
		calcRadius();
		break;
	default:
#ifdef CONSOLE_LOG
		std::cout << "<pLight::setProperty> Trying to set inexistant property.\n";
#endif
		break;
	}
}

void pLight::setUniforms(const Shader & shader) const {
	glUniform3f(shader.getUniform(_uniform + ".pos"), _pos.x, _pos.y, _pos.z);
	glUniform3f(shader.getUniform(_uniform + ".diffuse"), _diffuse.r, _diffuse.g, _diffuse.b);
	glUniform3f(shader.getUniform(_uniform + ".specular"), _specular.r, _specular.g, _specular.b);
	glUniform3f(shader.getUniform(_uniform + ".attenuation"), _attenuation.x, _attenuation.y, _attenuation.z);

	//we need to adjust the view matrix so the light sphere does not get cut off by the camera's far plane:
	float distance = glm::length(_cam->pos() - _pos);
	float farPlane = distance + _r;
	CamInfo cam = _cam->getMatrizes();


	//glm::mat4 P = glm::perspective(glm::radians(_cam->fov()), _cam->aspect(), _cam->nearPlane(), farPlane);
	//glUniformMatrix4fv(shader.getUniform("VP"), 1, GL_FALSE, glm::value_ptr(P * cam.V));
}

void pLight::draw(const Shader & shader) const {
	//glUniform3f(shader.getUniform("lightPos"), _pos.x, _pos.y, _pos.z);
	setUniforms(shader);

	/* determine if we are inside or outside the lights radius: */ 
	float distance = glm::length(_cam->pos() - _pos);
	if (distance < _r) {
		glCullFace(GL_FRONT);
	}
	else {
		glCullFace(GL_BACK);
	}

	/*
	if (_front) glCullFace(GL_FRONT);
	else glCullFace(GL_BACK);*/

	_sphere->drawNaked(shader);
}

void pLight::debugDraw(const Shader & shader) const {
	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	_sphere->draw(shader);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
}

void pLight::calcRadius() {
	/*	calculate the radius of the light based of the attenuation constants.
	forumla: attenuation = intensity / (constant + linear*distance + quadratic^2*distance)
	-> we assume the lights radius ends with an attenuation value of 1/256
	-> solve the quadratic equation

	to improve: take light intensity into account (we assume its 1 now) */


	float c = _attenuation.x;
	float l = _attenuation.y;
	float q = _attenuation.z;

	float det = l*l / (4.0*q*q) + ((256.0 - c) / q);
	_r = -l / (2.0 * q) + sqrtf(det);

#ifdef CONSOLE_LOG
	std::cout << "Point light sphere calculation:\n";
	std::cout << c << "/" << l << "/" << q << std::endl;
	std::cout << "Radius " << _r << std::endl;
#endif
}

}