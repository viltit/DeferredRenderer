/*	Class for representing 3D-Models loaded from an obj File 

	Model inherits from Scene Node and expands it by loading meshes and textures 
	as Child-Nodes from its constructor. It has no other tasks.					*/

#pragma once

#include <string>

#include "Scene.hpp"

namespace vitiGL {

class Mesh;

class Model : public SceneNode {
public:
	Model	(const std::string& filePath, 
			 bool textureFolder = true);
	~Model();

private:
	static int id;
};

}