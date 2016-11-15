#include "PhysicShape.hpp"

#include "PhysicObject.hpp"
#include <assert.h>

namespace vitiGEO {

PhysicShape::PhysicShape(PhysicObject* owner, const std::vector<glm::vec3>& vertices)
	:	_owner{ owner },
		_vertices{ vertices }
{
	assert(owner);
	/* calculate edges. We assume we have triangles. */
	for (size_t i = 0; i < _vertices.size(); i+=3) {
		glm::vec3 v1 = _vertices[i++];
		glm::vec3 v2 = _vertices[i++];
		glm::vec3 v3 = _vertices[i++];

		_edges.push_back(v2 - v1);
		_edges.push_back(v3 - v2);
		_edges.push_back(v1 - v3);
	}

	/* calculate edges normals: */

}


PhysicShape::~PhysicShape() {
}
}