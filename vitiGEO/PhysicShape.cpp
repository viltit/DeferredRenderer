#include "PhysicShape.hpp"

#include "PhysicObject.hpp"
#include <assert.h>

namespace vitiGEO {

PhysicShape::PhysicShape(PhysicObject* owner, const std::vector<glm::vec3>& vertices)
	:	_owner{ owner }
{
	assert(owner);

	_vertices = vertices;

	/* calculate edges and normals. We assume we have triangles. */
	for (size_t i = 0; i < _vertices.size();) {
		glm::vec3 v1 = _vertices[i++];
		glm::vec3 v2 = _vertices[i++];
		glm::vec3 v3 = _vertices[i++];

		_edges.push_back(v2 - v1);
		_edges.push_back(v3 - v2);
		_edges.push_back(v1 - v3);

		/* only take genuine normals: */
		bool takeIt = true;
		glm::vec3 normal = glm::normalize(glm::cross(v1, v2));
		for (size_t j = 0; j < _normals.size(); j++) {
			if (glm::dot(_normals[j], normal) > 1.0f - 0.001f) takeIt = false;
		}
		if (takeIt)
			_normals.push_back(normal);
	}

	/* we seem to have messed up the triangles orientation.... */
	std::cout << "NORMALS: " << _normals.size() << std::endl;
}


PhysicShape::~PhysicShape() {
}

void PhysicShape::normals(std::vector<glm::vec3>& axes) {
	glm::mat3 M = glm::toMat3(_owner->orientation());
	for (size_t i = 0; i < _normals.size(); i++) {
		axes.push_back(M * _normals[i]);
	}
}
}