/*	class to transform the game objects (rotate, scale, ...) 
	should act as intermediary between sceneNode and BulletPhysics	
	
	also, we add a couple of functions to get from bullets Vector3 to
	glm::vec3, and other data types
*/

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <bt/btBulletDynamicsCommon.h>

#include <iostream>

#include "Math.hpp"

namespace vitiGEO {

class Transform {
public:
	Transform()
		: _scale{ 1.0f, 1.0f, 1.0f }
	{}

	Transform(const glm::vec3& pos, const glm::vec3 scale = glm::vec3{ 1.0f, 1.0f, 1.0f })
		: _pos	  { pos },
		  _scale  { scale }
	{}

	~Transform() {};

	/* setters: */
	void	setPos(const glm::vec3& pos)	{ _pos = pos; }
	void	move(const glm::vec3& distance) { _pos += distance; }

	void	rotate(float angle, const glm::vec3& axis) {
		_o = glm::rotate(_o, glm::radians(angle), axis);
		_o = glm::normalize(_o);
	}

	void	rotate(const glm::quat& q) {
		_o = _o * q;
	}

	void	rotateTo(const glm::quat& q)		{ _o = q; }
	void	rotateTo(float angle, const glm::vec3& axis) {
		_o = glm::rotate(_o, glm::radians(angle), axis);
		_o = glm::normalize(_o);
	}

	void	setScale(const glm::vec3& scale)	{ _scale = scale; }

	/* getters: */
	glm::vec3 pos() const { return _pos; }
	glm::vec3 scale() const { return _scale; }

	const glm::mat4 worldMatrix()	const {
		glm::mat4 W{ };
		W = glm::scale(W, _scale);
		W = glm::toMat4(_o) * W;
		W = setTranslation(W, _pos);
		return W;
	}

	glm::quat orientation() const {
		return _o;
	}

private:
	glm::vec3 _pos;
	glm::vec3 _scale;
	glm::quat _o;
};

/*	Helper functions to convert bullet data to glm data and vice versa: */
btQuaternion glmQuatToBtQuat(const glm::quat& q);

btVector3 glmVecToBtVec(const glm::vec3& v);

glm::quat btQuatToGlmQuat(const btQuaternion& q);

glm::vec3 btVecToGlmVec(const btVector3& v);

/*	get Euler angles from quaternion: 
	(modfied code from https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles) */
glm::vec3 toEuler(const glm::quat q);


/* get Euler angle from btQuaternion: */
btVector3 toEuler(const btQuaternion& q);

}