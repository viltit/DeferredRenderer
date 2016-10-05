/*	-------------------------------------------------------------------------------------------------
	Class for representing 3D-Models loaded from an obj File 

	->	We overwrite the shapes draw functions and use the function uploadVertInd instead 
		of uploadVertices to implement indexed drawing

	------------------------------------------------------------------------------------------------- */

#pragma once

#include <string>

#include "Shape.hpp"

namespace vitiGL {

class Mesh : public Shape {
public:
	Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& textures);
	~Mesh();


protected:
	// Inherited via Shape (obsolete in this case...)
	virtual void initVertices(std::vector<Vertex>& vertices) override;

	glm::vec3 size;
	glm::vec2 uv;
};

}
