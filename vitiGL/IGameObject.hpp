/* interface for all game objects (ie objects that need to be renderered and / or moved around onscreen ) */

#pragma once

#include <glm/glm.hpp>

namespace vitiGL {

class Shader;

enum class ObjType {
	shape,
	plight,
	dlight,
	skybox,
	mesh
};

class IGameObject {
public:
	IGameObject(ObjType type) 
		: _type { type }
	{};

	virtual ~IGameObject() {};

	virtual void draw(const Shader& shader) const = 0;
	virtual void drawNaked(const Shader& shader) const	{ draw(shader); }

	virtual void setModelMatrix(const glm::mat4& M)		{ _M = M; }
	virtual glm::mat4 matrix() const					{ return _M; }

	ObjType type()										{ return _type; }

protected:
	glm::mat4	_M;
	ObjType		_type;
};
}
