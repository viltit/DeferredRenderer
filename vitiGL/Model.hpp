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

	/* wip, debug: */
	void drawAABB();

private:
	static int id;

	/* WIP: */
	std::vector<vitiGEO::AABB> _aabb;
	std::vector<AABBShape> _aabbShape;
};

}