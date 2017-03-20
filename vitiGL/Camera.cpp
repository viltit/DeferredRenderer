#include "Camera.hpp"

#include <iostream>
#include <glm/gtc/type_ptr.hpp>

#include "vitiTypes.hpp"

namespace vitiGL {

Camera::Camera(float aspect) 
	:	_aspect		{ aspect },
		_up			{ glm::vec3{0, 1, 0} },
		_worldUp	{ glm::vec3{ 0, 1, 0 } },
		_fov		{ 80.0f },
		_maxFov		{ 110.0f },
		_minFov		{ 10.0f },
		_pos		{ glm::vec3{ 0.0f, 0.0f, 0.0f } },
		_target		{ glm::vec3{ 0.0f, 0.0f, -1.0f } },
		_dpos		{ glm::vec3{ 0.0f, 0.0f, 0.0f } },
		_maxPitch	{ 5.0f },
		_near		{ 0.1f },
		_far		{ 100.0f },
		_speed		 { 0.1f },
		_sensitivity { 0.4f },
		_didUpdate	 { true }
{}

Camera::~Camera() {
}

void Camera::update() {
	_dir = glm::normalize(_target - _pos);

	//update camera vectors and quaternions:
	_right = glm::normalize(glm::cross(_dir, glm::vec3{ 0.0f, 1.0f, 0.0f }));
	_up = glm::normalize(glm::cross(_right, _dir));

	glm::vec3 axis = glm::cross(_dir, _worldUp);
	
	glm::quat pitchQ = glm::angleAxis(glm::radians(_pitch), axis);
	glm::quat headQ = glm::angleAxis(glm::radians(_yaw), _worldUp);
	glm::quat temp = glm::normalize(glm::cross(pitchQ, headQ));

	_dir = glm::rotate(temp, _dir);
	_pos += _dpos;
	_target = _pos + _dir;

	//smoother Movment:
	_yaw *= 0.5f;
	_pitch *= 0.5f;
	_dpos *= 0.8f;

	//calculate View and Perspective Matrix:
	_P = glm::perspective(glm::radians(_fov), _aspect, _near, _far);
	_V = glm::lookAt(_pos, _target, _up);
}

void Camera::move(Move m) {
	switch (m) {
	case Move::forward:
		_dpos += _dir * _speed;
		break;
	case Move::backward:
		_dpos -= _dir * _speed;
		break;
	case Move::left:
		_dpos -= glm::cross(_dir, _up) * _speed;
		break;
	case Move::right:
		_dpos += glm::cross(_dir, _up) * _speed;
		break;
	}
}

void vitiGL::Camera::rotate(int mouseX, int mouseY) {
	
	glm::vec3 mouseDelta = _mousePos - glm::vec3{ float(mouseX), float(mouseY), 0.0f };
	_mousePos = glm::vec3{ float(mouseX), float(mouseY), 0.0f };
	
	_yaw += mouseDelta.x * _sensitivity;
	
	//The pitch changes direction if the camera looks straight up or down:
	if ((_pitch > 90 && _pitch < 270) || (_pitch < -90 && _pitch > -270)) {
		_pitch -= mouseDelta.y * _sensitivity;
	}
	else {
		_pitch += mouseDelta.y * _sensitivity;
	}

	//check for boundaries:
	if (_pitch > 360.0f) _pitch -= 360.0f;
	else if (_pitch < -360.0f) _pitch += 360.0f;
	if (_yaw > 360.0f) _yaw -= 360.0f;
	else if (_yaw < -360.0f) _yaw += 360.0f;

}

void vitiGL::Camera::zoom(int z) {
	_fov -= z;
	if (_fov > _maxFov) _fov = _maxFov;
	if (_fov < _minFov) _fov = _minFov;
	_didUpdate = true;
}

void Camera::setVPUniform(const Shader& shader) const {
	glm::mat4 VP = _P * _V;
	glUniformMatrix4fv(shader.getUniform("VP"), 1, GL_FALSE, glm::value_ptr(VP));
}

void Camera::setUniforms(const Shader & shader) const {
	glm::mat4 VP = _P * _V;
	glUniformMatrix4fv(shader.getUniform("VP"), 1, GL_FALSE, glm::value_ptr(VP));
	glUniform3f(shader.getUniform("viewPos"), _pos.x, _pos.y, _pos.z);
}
}
