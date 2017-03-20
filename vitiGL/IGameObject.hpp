/* interface for all game objects (ie objects that need to be renderered and / or moved around onscreen ) */

#pragma once

#include <glm/glm.hpp>
#include <string>

namespace vitiGL {

class Shader;

enum class ObjType {
	shape,
	plight,
	dlight,
	skybox,
	mesh
};

/* we need this enum for saving the scene in an xml */
enum class ShapeType {
	tetrahedron,
	cuboid,
	octahedron,
	icosahedron,
	sphere,
	mesh	
};

class IGameObject {
public:
	IGameObject(ObjType type, const std::string& configFile) 
		: _type { type },
		  _configFile { configFile }
	{};

	virtual ~IGameObject() {};

	virtual void draw(const Shader& shader) const = 0;
	virtual void drawNaked(const Shader& shader) const	{ draw(shader); }

	virtual void setModelMatrix(const glm::mat4& M)		{ _M = M; }
	virtual glm::mat4 matrix() const					{ return _M; }

	ObjType type()										{ return _type; }
	std::string file() 									{ return _configFile; }


protected:
	glm::mat4	_M;
	ObjType		_type;
	std::string _configFile;
};
}
