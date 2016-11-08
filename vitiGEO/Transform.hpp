/*	class to transform the game objects (rotate, scale, ...) 
	
	should act as intermediary between sceneNode and Physics	
	
	TO DO: As soon as physics implements angular movment, update matrix
*/

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

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

	const glm::mat3& localMatrix3x3() const {
		if (!_validW) {
			_M = setTranslation(_M, _pos);
			_W = _M * _W;
			_validW = true;
		}
		/* possibly wrong: */
		glm::mat3 M3x3 = {
			_M[0][0], _M[1][0], _M[2][0],
			_M[0][1], _M[1][1], _M[2][1],
			_M[0][2], _M[1][2], _M[2][2],
		};

		std::cout
			<< "4x4 Matrix:\n"
			<< _M[0][0] << "/" << _M[1][0] << "/" << _M[2][0] << "/" << _M[3][0] << "\n"
			<< _M[0][1] << "/" << _M[1][1] << "/" << _M[2][1] << "/" << _M[3][1] << "\n"
			<< _M[0][2] << "/" << _M[1][2] << "/" << _M[2][2] << "/" << _M[3][2] << "\n"
			<< _M[0][3] << "/" << _M[1][3] << "/" << _M[2][3] << "/" << _M[3][3] << "\n"
			<< std::endl;

		std::cout
			<< "3x3 Matrix:\n"
			<< M3x3[0][0] << "/" << M3x3[1][0] << "/" << M3x3[2][0] << "\n"
			<< M3x3[0][1] << "/" << M3x3[1][1] << "/" << M3x3[2][1] << "\n"
			<< M3x3[0][2] << "/" << M3x3[1][2] << "/" << M3x3[2][2] << "\n"
			<< std::endl;

		std::cout << "position vector:\n"
			<< getTranslation(_M).x << "/" << getTranslation(_M).y << "/" << getTranslation(_M).z << std::endl;

		return M3x3;
	}

	const glm::mat4& worldMatrix()	const {
		if (!_validW) {
			_M = setTranslation(_M, _pos);
			_W = _M * _W;
			_validW = true;
		}
		return _W;
	}

private:
	mutable glm::mat4 _M;	//local space matrix
	mutable glm::mat4 _W;	//world space matrix

	mutable bool _validW;

	/* linear:*/
	glm::vec3 _pos;

};
}