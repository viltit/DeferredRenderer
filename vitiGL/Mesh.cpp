#include "Mesh.hpp"

#include "Error.hpp"
#include "Cache.hpp"

#include <vector>
#include <iostream>

namespace vitiGL {

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& textures) {

	numVertices = vertices.size();

	calcNormals(vertices);
	calcTangents(vertices);
	uploadVertices(vertices);

	tbo.resize(3);
	tbo = textures;
}


Mesh::~Mesh() {
}

void Mesh::initVertices(std::vector<Vertex>& vertices) {
}
}