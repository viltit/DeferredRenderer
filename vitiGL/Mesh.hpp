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
#include "AABBShape.hpp"

namespace vitiGL {

class Mesh : public ShapeI {
public:
	Mesh	(std::vector<Vertex>& vertices, 
			 std::vector<GLuint> indices, 
			 std::vector<std::pair<int, GLuint>>& textures,
			 Camera* cam);

	/* copy constructor: */
	Mesh(const Mesh& m);

	/* move constructor: */
	Mesh(Mesh&& mesh);

	~Mesh();

	void updateAABB() { aabb.transform(_M); _aabbShape->update(&aabb); }

	/* debug function: draw aaabb */
	void drawAABB() const { _aabbShape->draw(); }

	std::vector<glm::vec3>& vertices() { return _vertices; }

protected:
	// Inherited via Shape (obsolete in this case...)
	virtual void initVertices(std::vector<Vertex>& vertices) override;

	AABBShape* _aabbShape;

	std::vector<glm::vec3> _vertices;

	glm::vec3 size;
	glm::vec2 uv;
};

}
