/*	Class for representing 3D-Models loaded from an obj File 

	Model inherits from Scene Node and expands it by loading meshes and textures 
	as Child-Nodes from its constructor. It has no other tasks.					*/

#pragma once

#include <string>

#include "Scene.hpp"
#include "AABBShape.hpp"
#include <AABB.hpp>

namespace vitiGL {

class Mesh;
class Camera;

class Model : public SceneNode {
public:
	Model	(const std::string& filePath,
			 Camera* cam,
			 bool textureFolder = true);
	~Model();

	/* because we have an aabb, we need to update it too: */
	void update(const Uint32& deltaTime) override {
		/* calculate new position: */
		if (_parent) _W = _parent->posMatrix() * _M;
		else _W = _M;

		/* give world position to the shape for drawing: */
		if (_obj) _obj->setModelMatrix(_W);

		/* update all children: */
		for (auto& C : _children) C->update(deltaTime);

		/* update the aabb: */
		for (int i = 0; i < _aabb.size(); i++) {
			_aabb[i].transform(_W);
			_aabbShape[i].update(&_aabb[i]);
		}
	}

	/* wip, debug: */
	void drawAABB();

private:
	static int id;

	/* WIP: */
	std::vector<vitiGEO::AABB> _aabb;
	std::vector<AABBShape> _aabbShape;
};

}