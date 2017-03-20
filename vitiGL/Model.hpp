/*	Class for representing 3D-Models loaded from an obj File 

	Model inherits from Scene Node and expands it by loading meshes and textures 
	as Child-Nodes from its constructor. It has no other tasks.					*/

#pragma once

#include <string>

#include "Scene.hpp"
#include <AABB.hpp>

namespace vitiGL {

class Mesh;
class Camera;

class Model : public SceneNode {
public:
	Model	(const std::string& filePath,
			 bool textureFolder = true);
	~Model();

	/* wip, debug: */
	std::vector<vitiGEO::AABB*> aabb();
	std::vector<std::vector<glm::vec3>> vertices();

private:
	void loadFromCache(const std::string& filePath);
	void loadFromFile(const std::string& filePath, bool textureFolder);

	static int id;
};

}
