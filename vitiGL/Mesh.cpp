#include "Mesh.hpp"

#include "Error.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <vector>
#include <iostream>

namespace vitiGL {

Mesh::Mesh(const std::string& filePath) {
	//load object data with tinyObjLoader Library:
	tinyobj::attrib_t vertices;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string error;

	bool ret = tinyobj::LoadObj(&vertices, &shapes, &materials, &error, filePath.c_str());

	if (!error.empty()) std::cout << error << std::endl;
	if (!ret) vitiError(("<Mesh::Mesh>TinyObjLoader failed to load object " + filePath).c_str());

}


Mesh::~Mesh() {
}
}