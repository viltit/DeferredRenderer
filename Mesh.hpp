#pragma once

#include <string>
#include <vector>

#include "vitiTypes.hpp"

namespace vitiGL {

enum class MeshType {
	colored,
	specular,
	normal
};

/* Abstract base class for all Mesh types: */
class BaseMesh {
public:
	BaseMesh();
	~BaseMesh();

	virtual void draw() = 0;

private:
	virtual void initVertices(std::vector<Vertex>& vertices) = 0;
};

class MeshColored : public BaseMesh {
	virtual void initVertices(std::vector<Vertex>& vertices);
};

class MeshDiffuse : public BaseMesh {
	virtual void initVertices(std::vector<Vertex>& vertices);
};

class MeshDiffuse : public BaseMesh {
	virtual void initVertices(std::vector<Vertex>& vertices);
};
}