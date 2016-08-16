#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "vitiTypes.hpp"

namespace vitiGL {


struct CamInfo{
CamInfo	(const glm::mat4& P, const glm::mat4& V, 
		 const glm::vec3& pos, const glm::vec3& front, const glm::vec3 up, glm::vec3& right,
		 float fov, float near, float far, float aspect, bool updated)
	:	P{ P }, V{ V }, pos{ pos }, front{ front }, up{ up }, right { right },
		fov { fov }, near{ near }, far{ far }, aspect{ aspect }, updated{ updated }
{}

glm::mat4 P;
glm::mat4 V;
glm::vec3 pos;
glm::vec3 front;
glm::vec3 up;
glm::vec3 right;
float fov;
float near;
float far;
float aspect;
bool updated;
};

class Camera {
public:
	Camera(float aspect);
	~Camera();

	void		update();

	void		move(Move m);
	void		rotate(int mouseX, int mouseY);
	void		zoom(int z);

	/* getters ans setters, all inline: */
	CamInfo		getMatrizes()  { 
		CamInfo temp = CamInfo{ _P, _V, _pos, _dir, _up, _right, _fov, _near, _far, _aspect, _didUpdate };
		_didUpdate = false; 
		return temp;
	}

	void		setTarget(const glm::vec3& target) { _target = target; }

	void		setClipping(float near, float far) { _near = near; _far = far; }
	void		setViewport(float aspect) { _aspect = aspect; }

	float		sensitivity() const { return _sensitivity; }
	void		setSensitivity(float s) { _sensitivity = s; }

	float		speed() const { return _speed; }
	void		setSpeed(float s) { _speed = s; }

	glm::vec3	pos() const	{ return _pos; }
	void		setPos(glm::vec3 pos) { _pos = pos; }

	float		fov() const { return _fov; }
	void		setFov(float f) { _fov = f; }

	float		nearPlane() const	{ return _near; }
	float		farPlane() const	{ return _far; }
	float		aspect() const	{ return _aspect; }

private:
	float		_aspect;		// viewport aspect ratio
	float		_near;			// near and far clipping plane
	float		_far;

	float		_fov;			// field of view
	float		_maxFov;
	float		_minFov;

	float		_yaw;
	float		_pitch;
	float		_maxPitch;

	bool		_didUpdate;		//marks if the perspective matrix was updated

	glm::vec3	_pos;
	glm::vec3	_dpos;
	glm::vec3	_target;
	glm::vec3	_dir;
	glm::vec3	_right;
	glm::vec3	_up;
	glm::vec3	_worldUp;
	glm::quat	_rotation;

	glm::vec3	_mousePos;

	glm::mat4	_P;				// projection matrix
	glm::mat4	_V;				// view matrix

	float		_speed;			// movment speed
	float		_sensitivity;	// rotation speed

};
}
