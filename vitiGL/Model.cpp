#include "Model.hpp"

#include "Mesh.hpp"
#include "Error.hpp"
#include "Cache.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <iostream>

namespace vitiGL {

Model::Model(const std::string& filePath) 
	: IGameObject{ ObjType::model }
{
	std::string basePath = filePath.substr(0, filePath.find_last_of("/"));
	basePath += "/";
	/* load object data with tinyObjLoader Library: */
	tinyobj::attrib_t attribs;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string error;

	bool ret = tinyobj::LoadObj(&attribs, &shapes, &materials, &error, filePath.c_str());

	if (!error.empty()) std::cout << error << std::endl;
	if (!ret) throw vitiError(("<Mesh::Mesh>TinyObjLoader failed to load object " + filePath).c_str());

	/* create a mesh for each tinyobj::shape: */
	int i{ 0 };
	for (const auto& shape : shapes) {
		std::vector<Vertex> vertices;
		std::vector<GLuint> textures;
		for (const auto& index : shape.mesh.indices) {
			Vertex vertex;
			vertex.pos = {
				attribs.vertices[3 * index.vertex_index + 0],
				attribs.vertices[3 * index.vertex_index + 1],
				attribs.vertices[3 * index.vertex_index + 2],
			};
			vertex.uv = {
				attribs.texcoords[2 * index.texcoord_index + 0],
				attribs.texcoords[2 * index.texcoord_index + 1]
			};
			vertices.push_back(vertex);
		}
		if (materials[i].specular_texname == "") materials[i].specular_texname = materials[i].diffuse_texname;

		try {
			textures.push_back(Cache::getTexture(basePath + materials[i].diffuse_texname));
		}
		catch (...) {
			textures.push_back(Cache::getTexture("Textures/MetalFloorsBare_Diffuse.png"));
		}
		try {
			textures.push_back(Cache::getTexture(basePath + materials[i].specular_texname));
		}
		catch (...) {
			textures.push_back(Cache::getTexture("Textures/MetalFloorsBare_Specular.png"));
		}
		try {
			textures.push_back(Cache::getTexture(basePath + materials[i].normal_texname));
		}
		catch (...) {
			textures.push_back(Cache::getTexture("Textures/MetalFloorsBare_Normal.png"));
		}

		Mesh mesh(vertices, textures);
	}
}


Model::~Model() {
}

void Model::setModelMatrix(const glm::mat4 & M) {
	 _M = M;
	 for (auto& mesh : _mesh) mesh.setModelMatrix(M);
}

void Model::draw(const Shader & shader) const {
	std::cout << _mesh.size() << std::endl;
	int i{ 0 };
	for (const auto& mesh : _mesh) {
		std::cout << "Drawing mesh no " << i << std::endl;
		mesh.draw(shader);
	}
}

void Model::drawNaked(const Shader & shader) const {
	int i{ 0 };
	for (const auto& mesh : _mesh) {
		std::cout << "Drawing mesh no " << i << std::endl;
		mesh.drawNaked(shader);
	}
}

}