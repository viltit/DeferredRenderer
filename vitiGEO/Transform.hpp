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
		: _validW { false }
	{}

	Transform(const glm::vec3& pos) 
		: _validW { false },
		  _pos	  { pos }
	{}

	~Transform() {};

	/* setters: */
	void	setPos(const glm::vec3& pos)	{ _pos = pos; _validW = false; }
	void	move(const glm::vec3& distance) { _pos += distance; _validW = false; }

	void	rotate(float angle, const glm::vec3& axis) {
		_M = glm::rotate(_M, glm::radians(angle), axis);
		_validW = false;
	}

	void	rotate(const glm::quat& q) {
		glm::mat4 R = glm::toMat4(q);
		_M = R * _M;
		_validW = false;
	}

	void	rotateTo(const glm::quat& q) {
		glm::quat o = orientation();
		rotate(q * glm::inverse(o));
	}

	void	scale(const glm::vec3& scale) { _M = glm::scale(_M, scale); _validW = false; }

	void	setWorldMatrix(const glm::mat4& W) { _W = W; }

	/* getters: */
	const glm::vec3& pos() const { return _pos; }

	const glm::mat4& localMatrix() const {
		if (!_validW) {
			_M = setTranslation(_M, _pos);
			_W = _M * _W;
			_validW = true;
		}
		return _M;
	}

	const glm::mat4& worldMatrix()	const {
		if (!_validW) {
			_M = setTranslation(_M, _pos);
			_W = _M * _W;
			_validW = true;
		}
		return _W;
	}

	glm::quat orientation() const {
		if (!_validW) {
			_M = setTranslation(_M, _pos);
			_W = _M * _W;
			_validW = true;
		}
		return glm::quat(_W);
	}

private:
	mutable glm::mat4 _M;	//local space matrix
	mutable glm::mat4 _W;	//world space matrix

	mutable bool _validW;

	/* linear:*/
	glm::vec3 _pos;

};

/*	Helper functions to convert bullet data to glm data and vice versa: */
btQuaternion glmQuatToBtQuat(const glm::quat& q);

btVector3 glmVecToBtVec(const glm::vec3& v);

glm::quat btQuatToGlmQuat(const btQuaternion& q);

glm::vec3 btVecToGlmVec(const btVector3& v);

}