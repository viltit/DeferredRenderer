#include "Mesh.hpp"

#include "Error.hpp"
#include "Cache.hpp"

#include <vector>
#include <iostream>

namespace vitiGL {

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<GLuint> indices, std::vector<std::pair<int, GLuint>>& textures, Camera* cam)
	:	ShapeI	{ ObjType::mesh }
		
{
	/* WIP: add aabb and aabbShape: */
	_vertices.resize(indices.size());
	for (int i = 0; i < indices.size(); i++) {
		_vertices.push_back(vertices[indices[i]].pos);
	}
	aabb.construct(_vertices);
	_aabbShape = new AABBShape{ &aabb, cam };

	numVertices = indices.size();

	calcNormals(vertices, indices);
	calcTangents(vertices, indices);
	normalizeSeam(vertices, indices);
	uploadVertices(vertices, indices);

	for (const auto& T : textures) {
		switch (T.first) {
		case TEXTURE_DIFFUSE:
			material.setTexture(TEXTURE_DIFFUSE, T.second);
			break;
		case TEXTURE_SPECULAR:
			material.setTexture(TEXTURE_SPECULAR, T.second);
			break;
		case TEXTURE_NORMAL:
			material.setTexture(TEXTURE_NORMAL, T.second);
			break;
		case TEXTURE_BUMP:
			material.setTexture(TEXTURE_BUMP, T.second);
			break;
		case TEXTURE_ALPHA:
			material.setTexture(TEXTURE_ALPHA, T.second);
			break;
		default:
			throw vitiError("<Mesh::Mesh> Invalid Texture index.");
		}
	}
}

Mesh::Mesh(const Mesh& mesh) 
{
	vbo = mesh.vbo;
	vao = mesh.vao;
	ebo = mesh.ebo;
	material = mesh.material;
	numVertices = mesh.numVertices;
	aabb = mesh.aabb;
	_aabbShape = mesh._aabbShape;
	_vertices = mesh._vertices;
}

Mesh::Mesh(Mesh && mesh) {
	vbo = mesh.vbo;
	vao = mesh.vao;
	material = mesh.material;
	ebo = mesh.ebo;
	numVertices = mesh.numVertices;
	aabb = mesh.aabb;
	_aabbShape = mesh._aabbShape;
	_vertices = mesh._vertices;

	mesh.vbo = 0;
	mesh.vao = 0;
	mesh.ebo = 0;
}

Mesh::~Mesh() {
	//if (vao) glDeleteVertexArrays(1, &vao);
	//if (vbo) glDeleteBuffers(1, &vbo);
}

void Mesh::initVertices(std::vector<Vertex>& vertices) {
}
}
