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
	/* TO DO: Every Shape (or every IGameObj??) has an aabb */
	void update(const Uint32& deltaTime) override;

	/* wip, debug: */
	void drawAABB();
	std::vector<vitiGEO::AABB*> aabb();
	std::vector<std::vector<glm::vec3>> vertices();

private:
	static int id;
};

}