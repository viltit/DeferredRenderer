#include "Mesh.hpp"

#include "Error.hpp"
#include "Cache.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <vector>
#include <iostream>
#include <conio.h>

namespace vitiGL {

Mesh::Mesh(const std::string& filePath) {
	/* load object data with tinyObjLoader Library: */
	tinyobj::attrib_t attribs;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string error;

	bool ret = tinyobj::LoadObj(&attribs, &shapes, &materials, &error, filePath.c_str());

	if (!error.empty()) std::cout << error << std::endl;
	if (!ret) throw vitiError(("<Mesh::Mesh>TinyObjLoader failed to load object " + filePath).c_str());


	/* convert the data into our own structures:
		(we loose indexing on the way, but we have no support for it atm) */
	std::vector<Vertex> myVertices;

	for (const auto& shape : shapes) {
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
			myVertices.push_back(vertex);
		}
	}
	numVertices = myVertices.size();

	calcNormals(myVertices);

	calcTangents(myVertices);
	uploadVertices(myVertices);

	//Textures?
	tbo.resize(3);
	tbo[0] = Cache::getTexture("Textures/MetalFloorsBare_Diffuse.png");
	tbo[1] = Cache::getTexture("Textures/MetalFloorsBare_Specular.png");
	tbo[2] = Cache::getTexture("Textures/MetalFloorsBare_Normal.png");
}


Mesh::~Mesh() {
}

void Mesh::initVertices(std::vector<Vertex>& vertices) {
	//we will need this variables at a later point:
	float w = size.x / 2.0f;
	float h = size.y / 2.0f;
	float d = size.z / 2.0f;

	float uv_w = uv.x;
	float uv_h = uv.y;


	vertices.resize(36);

	vertices[numVertices].pos = glm::vec3{ -w, -h,  d };	vertices[numVertices++].uv = glm::vec2{ 0.0f, 0.0f };	/* front */
	vertices[numVertices].pos = glm::vec3{ w, -h,  d };		vertices[numVertices++].uv = glm::vec2{ uv_w, 0.0f };
	vertices[numVertices].pos = glm::vec3{ w,  h,  d };		vertices[numVertices++].uv = glm::vec2{ uv_w, uv_h };
	vertices[numVertices].pos = glm::vec3{ w,  h,  d };		vertices[numVertices++].uv = glm::vec2{ uv_w, uv_h };
	vertices[numVertices].pos = glm::vec3{ -w,  h,  d };	vertices[numVertices++].uv = glm::vec2{ 0.0f, uv_h };
	vertices[numVertices].pos = glm::vec3{ -w, -h,  d };	vertices[numVertices++].uv = glm::vec2{ 0.0f, 0.0f };

	vertices[numVertices].pos = glm::vec3{ -w,  h,  d };	vertices[numVertices++].uv = glm::vec2{ 0.0f, uv_h };	/* left */
	vertices[numVertices].pos = glm::vec3{ -w,  h, -d };	vertices[numVertices++].uv = glm::vec2{ uv_w, uv_h };
	vertices[numVertices].pos = glm::vec3{ -w, -h, -d };	vertices[numVertices++].uv = glm::vec2{ uv_w, 0.0f };
	vertices[numVertices].pos = glm::vec3{ -w, -h, -d };	vertices[numVertices++].uv = glm::vec2{ uv_w, 0.0f };
	vertices[numVertices].pos = glm::vec3{ -w, -h,  d };	vertices[numVertices++].uv = glm::vec2{ 0.0f, 0.0f };
	vertices[numVertices].pos = glm::vec3{ -w,  h,  d };	vertices[numVertices++].uv = glm::vec2{ 0.0f, uv_h };

	vertices[numVertices].pos = glm::vec3{ w,  h, -d };		vertices[numVertices++].uv = glm::vec2{ uv_w, uv_h };	/* back */
	vertices[numVertices].pos = glm::vec3{ w, -h, -d };		vertices[numVertices++].uv = glm::vec2{ uv_w, 0.0f };
	vertices[numVertices].pos = glm::vec3{ -w, -h, -d };	vertices[numVertices++].uv = glm::vec2{ 0.0f, 0.0f };
	vertices[numVertices].pos = glm::vec3{ -w, -h, -d };	vertices[numVertices++].uv = glm::vec2{ 0.0f, 0.0f };
	vertices[numVertices].pos = glm::vec3{ -w,  h, -d };	vertices[numVertices++].uv = glm::vec2{ 0.0f, uv_h };
	vertices[numVertices].pos = glm::vec3{ w,  h, -d };		vertices[numVertices++].uv = glm::vec2{ uv_w, uv_h };

	vertices[numVertices].pos = glm::vec3{ w, -h, -d };		vertices[numVertices++].uv = glm::vec2{ uv_w, 0.0f };	/* right */
	vertices[numVertices].pos = glm::vec3{ w,  h, -d };		vertices[numVertices++].uv = glm::vec2{ uv_w, uv_h };
	vertices[numVertices].pos = glm::vec3{ w,  h,  d };		vertices[numVertices++].uv = glm::vec2{ 0.0f, uv_h };
	vertices[numVertices].pos = glm::vec3{ w,  h,  d };		vertices[numVertices++].uv = glm::vec2{ 0.0f, uv_h };
	vertices[numVertices].pos = glm::vec3{ w, -h,  d };		vertices[numVertices++].uv = glm::vec2{ 0.0f, 0.0f };
	vertices[numVertices].pos = glm::vec3{ w, -h, -d };		vertices[numVertices++].uv = glm::vec2{ uv_w, 0.0f };

	vertices[numVertices].pos = glm::vec3{ w,  h,  d };		vertices[numVertices++].uv = glm::vec2{ uv_w, 0.0f };	/* top */
	vertices[numVertices].pos = glm::vec3{ w,  h, -d };		vertices[numVertices++].uv = glm::vec2{ uv_w, uv_h };
	vertices[numVertices].pos = glm::vec3{ -w,  h, -d };	vertices[numVertices++].uv = glm::vec2{ 0.0f, uv_h };
	vertices[numVertices].pos = glm::vec3{ -w,  h, -d };	vertices[numVertices++].uv = glm::vec2{ 0.0f, uv_h };
	vertices[numVertices].pos = glm::vec3{ -w,  h,  d };	vertices[numVertices++].uv = glm::vec2{ 0.0f, 0.0f };
	vertices[numVertices].pos = glm::vec3{ w,  h,  d };		vertices[numVertices++].uv = glm::vec2{ uv_w, 0.0f };

	vertices[numVertices].pos = glm::vec3{ -w, -h, -d };	vertices[numVertices++].uv = glm::vec2{ uv_w, 0.0f };	/* bottom */
	vertices[numVertices].pos = glm::vec3{ w, -h, -d };		vertices[numVertices++].uv = glm::vec2{ uv_w, uv_h };
	vertices[numVertices].pos = glm::vec3{ w, -h,  d };		vertices[numVertices++].uv = glm::vec2{ 0.0f, uv_h };
	vertices[numVertices].pos = glm::vec3{ w, -h,  d };		vertices[numVertices++].uv = glm::vec2{ 0.0f, uv_h };
	vertices[numVertices].pos = glm::vec3{ -w, -h,  d };	vertices[numVertices++].uv = glm::vec2{ 0.0f, 0.0f };
	vertices[numVertices].pos = glm::vec3{ -w, -h, -d };	vertices[numVertices++].uv = glm::vec2{ uv_w, 0.0f };
}
}