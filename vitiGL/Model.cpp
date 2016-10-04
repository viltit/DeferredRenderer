/* This class loads wavefront obj files with the help of the tinyObjLoader */

#include "Model.hpp"

#include "Mesh.hpp"
#include "Error.hpp"
#include "Cache.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <iostream>

namespace vitiGL {

/* TO DO: 
	
	- check if normals are provided and only calculate them if not
	- check if textures are inside the same folder as the .obj file (ie: Cache::getTexture(basePath + ...))
	- provide standard normal map AND OR allow no-normal-map in shape.draw()
	- find out what is off with the glasses when loading "nanosuit.obj"

*/

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

	bool ret = tinyobj::LoadObj(&attribs, &shapes, &materials, &error, filePath.c_str(), basePath.c_str());

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

			/* TO DO: Check if normals are delivered

			vertex.normal = {
				attribs.normals[3 * index.normal_index + 0],
				attribs.normals[3 * index.normal_index + 1],
				attribs.normals[3 * index.normal_index + 2],
			};*/

			vertices.push_back(vertex);
		}

		/* get the textures: */
		std::cout << "Material properties:\n";
		std::cout << "--------------------\n";
		std::cout << "Alpha texture: " << materials[i].alpha_texname << std::endl;
		std::cout << "Ambient texture: " << materials[i].ambient_texname << std::endl;
		std::cout << "Bump texture: " << materials[i].bump_texname << std::endl;
		std::cout << "Diffuse texture: " << materials[i].diffuse_texname << std::endl;
		std::cout << "Normal texture: " << materials[i].normal_texname << std::endl;
		std::cout << "Displacement texture: " << materials[i].displacement_texname << std::endl;
		std::cout << "Specular texture: " << materials[i].specular_texname << std::endl;
		std::cout << "Emissive texture: " << materials[i].emissive_texname << std::endl;
		std::cout << "Roughness texture: " << materials[i].roughness_texname << std::endl;
		std::cout << "Sheen texture: " << materials[i].sheen_texname << std::endl;
		std::cout << "Specular Highlight texture: " << materials[i].specular_highlight_texname << std::endl;
		std::cout << "Metallic texture: " << materials[i].metallic_texname << std::endl;

		//if specular texture is not found, use diffuse:
		if (materials[i].specular_texname == "") materials[i].specular_texname = materials[i].diffuse_texname;

		//diffuse tex
		try {
			std::cout << "trying to open texture " << basePath + materials[i].diffuse_texname << std::endl;
			textures.push_back(Cache::getTexture(/*basePath + */materials[i].diffuse_texname));
		}
		catch (fileError) {
			textures.push_back(Cache::getTexture("Textures/MetalFloorsBare_Diffuse.png"));
		}

		//specular tex
		try {
			std::cout << "trying to open texture " << basePath + materials[i].specular_texname  << std::endl;
			textures.push_back(Cache::getTexture(/*basePath + */materials[i].specular_texname));
		}
		catch (fileError) {
			textures.push_back(Cache::getTexture(/*basePath + */materials[i].diffuse_texname));
		}

		//normal tex
		try {
			std::cout << "trying to open texture " << basePath + materials[i].bump_texname << std::endl;
			textures.push_back(Cache::getTexture(/*basePath + */materials[i].bump_texname));
		}
		catch (fileError) {
			textures.push_back(Cache::getTexture(/*basePath + */materials[i].bump_texname));
		}

		Mesh mesh(vertices, textures);
		_mesh.push_back(mesh);

		if (i < materials.size() - 1) i++;
	}
}


Model::~Model() {
}

void Model::setModelMatrix(const glm::mat4 & M) {
	 _M = M;
	 for (auto& mesh : _mesh) mesh.setModelMatrix(M);
}

void Model::draw(const Shader & shader) const {
	for (const auto& mesh : _mesh) {
		mesh.draw(shader);
	}
}

void Model::drawNaked(const Shader & shader) const {
	for (const auto& mesh : _mesh) {
		mesh.drawNaked(shader);
	}
}

}