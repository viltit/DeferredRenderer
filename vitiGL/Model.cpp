/* This class loads wavefront obj files with the help of the tinyObjLoader */

#include "Model.hpp"

#include "Mesh.hpp"
#include "Error.hpp"
#include "Cache.hpp"
#include "vitiTypes.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <iostream>
#include <unordered_map>

namespace vitiGL {

int Model::id = 0;

Model::Model(const std::string& filePath, bool textureFolder) {
	if (Cache::isModelLoaded(filePath)) loadFromCache(filePath);
	else loadFromFile(filePath, textureFolder);
}


Model::~Model() {
}

std::vector<vitiGEO::AABB*> Model::aabb() {
	std::vector<vitiGEO::AABB*> temp;
	for (auto& C : _children) {
		Mesh* mesh = static_cast<Mesh*>(C->obj());
		temp.push_back(mesh->getAABB());
	}

	return temp;
}

std::vector<std::vector<glm::vec3>> Model::vertices() {
	std::vector<std::vector<glm::vec3>> vertices;
	for (auto& C : _children) {
		Mesh* mesh = static_cast<Mesh*>(C->obj());
		vertices.push_back(mesh->vertices());
	}
	return vertices;
}

void Model::loadFromCache(const std::string & filePath) {
	ModelData modelData = Cache::pullModel(filePath);

	assert(modelData.vertices.size() == modelData.indices.size() == modelData.textures.size());

	if (modelData.vertices.size() == 1) {
		Mesh* mesh = new Mesh{ filePath, modelData.vertices[0], modelData.indices[0], modelData.textures[0] };
		_obj = mesh;
	}
	else {
		for (size_t i = 0; i < modelData.vertices.size(); i++) {
			Mesh* mesh = new Mesh{ filePath, modelData.vertices[0], modelData.indices[0], modelData.textures[0] };
			std::string meshName = _name + ".child" + std::to_string(++id);
			SceneNode* child = new SceneNode{ meshName, mesh, glm::vec3{} };
			addChild(child);
		}
	}
}

void Model::loadFromFile(const std::string & filePath, bool textureFolder) {
#ifdef CONSOLE_LOG
	std::cout << "\nSTART PROCESSING OBJ FILE " << filePath << std::endl;
	std::cout << "------------------------------------------------------\n";
#endif
	/* get models base path: */
	std::string basePath = filePath.substr(0, filePath.find_last_of("/"));
	basePath += "/";

	/* load object data with tinyObjLoader Library: */
	tinyobj::attrib_t attribs;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string error;

	bool ret = tinyobj::LoadObj(&attribs, &shapes, &materials, &error, filePath.c_str(), basePath.c_str());

	//reset base path if textures are in the same folder as the obj file:
	//TO DO: THIS IS BUGGED IN SOME SITUATIONS
	if (!textureFolder) basePath = "";

	if (!error.empty()) std::cout << error << std::endl;
	if (!ret) throw vitiError(("<Mesh::Mesh>TinyObjLoader failed to load object " + filePath).c_str());

	/* mark if we have one or several meshes in the object: */
	bool hasChildren = true;
	if (shapes.size() == 1) hasChildren = false;

	/* prepare variables to put the model in the cache: */
	ModelData modelData;

	/*	create a vitiGL::Mesh for each tinyobj::shape: */
	for (const auto& shape : shapes) {
		std::cout << "Processing a shape named " << shape.name << std::endl;
		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;
		std::vector<std::pair<int, GLuint>> textures;
		std::unordered_map<Vertex, int> uniqueVertices;

		for (const auto& index : shape.mesh.indices) {
			Vertex vertex;
			vertex.pos = {
				attribs.vertices[3 * index.vertex_index + 0],
				attribs.vertices[3 * index.vertex_index + 1],
				attribs.vertices[3 * index.vertex_index + 2],
			};
			vertex.uv = {
				attribs.texcoords[2 * index.texcoord_index + 0],
				1.0f - attribs.texcoords[2 * index.texcoord_index + 1]
			};

			/* TO DO: Check if normals are delivered
			vertex.normal = {
			attribs.normals[3 * index.normal_index + 0],
			attribs.normals[3 * index.normal_index + 1],
			attribs.normals[3 * index.normal_index + 2],
			};*/

			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = vertices.size();
				vertices.push_back(vertex);
			}
			indices.push_back(GLuint(uniqueVertices[vertex]));
		}

		/* get the textures:
		std::cout << "Material properties:\n";
		std::cout << "--------------------\n";
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
		std::cout << "Metallic texture: " << materials[i].metallic_texname << std::endl; */

		/* find the material name corresponding to this shape: */
		int matIndex = shape.mesh.material_ids[0];

		//if specular texture is not found, use diffuse:
		std::cout << "Alpha texture: " << materials[matIndex].alpha_texname << std::endl;

		if (matIndex >= 0) {
			//diffuse tex
			try {
				std::cout << "Trying to open diffuse texture file " << basePath + materials[matIndex].diffuse_texname << std::endl;
				textures.push_back(std::make_pair(
					TEXTURE_DIFFUSE,
					Cache::getTexture(basePath + materials[matIndex].diffuse_texname))
				);
			}
			catch (fileError) {
			}
			//specular tex:
			try {
				std::cout << "Trying to open specular texture file " << basePath + materials[matIndex].specular_texname << std::endl;
				textures.push_back(std::make_pair(
					TEXTURE_SPECULAR,
					Cache::getTexture(basePath + materials[matIndex].specular_texname))
				);
			}
			catch (fileError) {
			}
			//normal tex:
			try {
				std::cout << "Trying to open normal texture file " << basePath + materials[matIndex].bump_texname << std::endl;
				textures.push_back(std::make_pair(
					TEXTURE_NORMAL,
					Cache::getTexture(basePath + materials[matIndex].bump_texname))
				);
			}
			catch (fileError) {
			}
			/* alpha mask texture: */
			try {
				std::cout << "Trying to open alpha texture file " << basePath + materials[matIndex].alpha_texname << std::endl;
				textures.push_back(std::make_pair(
					TEXTURE_ALPHA,
					Cache::getTexture(basePath + materials[matIndex].alpha_texname))
				);
			}
			catch (fileError) {
			}
		}

		/* if we only have one mesh, the model directly contains the mesh: */
		if (!hasChildren) {
			Mesh* mesh = new Mesh{ filePath, vertices, indices, textures };
			_obj = mesh;
		}

		else {
			/* if we have several meshes, they are children of the model: */
			Mesh* mesh = new Mesh{ filePath, vertices, indices, textures };
			std::string meshName = shape.name;
			if (meshName == "") meshName = "Mesh_" + std::to_string(++id);
			else meshName += "_" + std::to_string(++id);

			SceneNode* child = new SceneNode{ meshName, mesh, glm::vec3{} };

			addChild(child);
		}

		/* store the values for the cache */
		modelData.vertices.push_back(vertices);
		modelData.indices.push_back(indices);
		modelData.textures.push_back(textures);
	}
	std::cout << "END PROCESSING OBJ FILE " << filePath << std::endl << std::endl;

	/* put the model in the cache: */
	Cache::pushModel(filePath, modelData);
}
}
