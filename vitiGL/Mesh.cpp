#include "Mesh.hpp"

#include "Error.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <vector>
#include <iostream>

namespace vitiGL {

Mesh::Mesh(const std::string& filePath) {
	/* load object data with tinyObjLoader Library: */
	tinyobj::attrib_t vertices;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string error;

	bool ret = tinyobj::LoadObj(&vertices, &shapes, &materials, &error, filePath.c_str());

	if (!error.empty()) std::cout << error << std::endl;
	if (!ret) throw vitiError(("<Mesh::Mesh>TinyObjLoader failed to load object " + filePath).c_str());


	/* convert the data into our own structures:
		(we loose indexing on the way, but we have no support for it atm) */

	std::vector<Vertex> myVertices(shapes[0].mesh.indices.size() / 3);
	int j{ 0 };
	for (int i = 0; i < shapes[0].mesh.indices.size(); i+=3) {
		myVertices[j].pos.x = vertices.vertices[shapes[0].mesh.indices[i].vertex_index];
		myVertices[j].pos.y = vertices.vertices[shapes[0].mesh.indices[i+1].vertex_index];
		myVertices[j].pos.z = vertices.vertices[shapes[0].mesh.indices[i+2].vertex_index];

		myVertices[j].uv.x = vertices.texcoords[shapes[0].mesh.indices[i].texcoord_index];
		myVertices[j].uv.y = vertices.texcoords[shapes[0].mesh.indices[i+1].texcoord_index];

		j++;
	}
	//check if normals are provided:
	j = 0;
	for (int i = 0; i < shapes[0].mesh.indices.size() && vertices.normals.size() != 0; i += 3) {
		myVertices[j].normal.x = vertices.normals[shapes[0].mesh.indices[i].vertex_index];
		myVertices[j].normal.y = vertices.normals[shapes[0].mesh.indices[i + 1].vertex_index];
		myVertices[j].normal.z = vertices.normals[shapes[0].mesh.indices[i + 2].vertex_index];

		j++;
	}

	//if no normals are provided, we need to calculate them:
	if (vertices.normals.size() == 0) calcNormals(myVertices);

	calcTangents(myVertices);
	uploadVertices(myVertices);

	//textures??? Model with several meshes??
}


Mesh::~Mesh() {
}

void Mesh::initVertices(std::vector<Vertex>& vertices) {
}
}