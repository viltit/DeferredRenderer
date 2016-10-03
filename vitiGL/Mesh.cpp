#include "Mesh.hpp"

#include "Error.hpp"
#include "Cache.hpp"

#include <vector>
#include <iostream>

namespace vitiGL {

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<int>& indices, std::vector<GLuint>& textures)
	:	ShapeI()
{

	numVertices = vertices.size();

	//calcNormals(vertices);
	calcTangents(vertices, indices);
	uploadVertices(vertices, indices);

	tbo.resize(3);
	tbo = textures;
}


Mesh::~Mesh() {
	if (vao) glDeleteVertexArrays(1, &vao);
	if (vbo) glDeleteBuffers(1, &vbo);
	if (ebo) glDeleteBuffers(1, &ebo);
}

void Mesh::initVertices(std::vector<Vertex>& vertices) {
}

}