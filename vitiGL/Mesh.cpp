#include "Mesh.hpp"

#include "Error.hpp"
#include "Cache.hpp"

#include <vector>
#include <iostream>

namespace vitiGL {

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<GLuint> indices, std::vector<GLuint>& textures)
{
	numVertices = indices.size();

	calcNormals(vertices, indices);
	calcTangents(vertices, indices);
	normalizeSeam(vertices, indices);
	uploadVertices(vertices, indices);

	tbo.resize(3);
	tbo = textures;
}

Mesh::Mesh(const Mesh& mesh) {
	vbo = mesh.vbo;
	vao = mesh.vao;
	ebo = mesh.ebo;
	tbo = mesh.tbo;
	numVertices = mesh.numVertices;
}

Mesh::Mesh(Mesh && mesh) {
	vbo = mesh.vbo;
	vao = mesh.vao;
	tbo = mesh.tbo;
	ebo = mesh.ebo;
	numVertices = mesh.numVertices;

	mesh.vbo = 0;
	mesh.vao = 0;
	mesh.ebo = 0;
	mesh.tbo.clear();
}

Mesh::~Mesh() {
	//if (vao) glDeleteVertexArrays(1, &vao);
	//if (vbo) glDeleteBuffers(1, &vbo);
}

void Mesh::initVertices(std::vector<Vertex>& vertices) {
}
}
