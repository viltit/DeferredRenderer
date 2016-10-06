/*	-------------------------------------------------------------------------------------------------
	Class for representing 3D-Models loaded from an obj File 

	->	We overwrite the shapes draw functions and use the function uploadVertInd instead 
		of uploadVertices to implement indexed drawing
	->  We define a copy and move constructor so we can return a Mesh from a function without
		the vao and tbo being deleted

	------------------------------------------------------------------------------------------------- */

#pragma once

#include <string>

#include "Shape.hpp"

namespace vitiGL {

class Mesh : public ShapeI {
public:
	Mesh(std::vector<Vertex>& vertices, std::vector<GLuint> indices, std::vector<GLuint>& textures);

	/* copy constructor: */
	Mesh(const Mesh& m);

	/* move constructor: */
	Mesh(Mesh&& mesh);

	~Mesh();


protected:
	// Inherited via Shape (obsolete in this case...)
	virtual void initVertices(std::vector<Vertex>& vertices) override;

	glm::vec3 size;
	glm::vec2 uv;
};

}
