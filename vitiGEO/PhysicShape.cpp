#include "PhysicShape.hpp"

#include "PhysicObject.hpp"
#include <assert.h>

namespace vitiGEO {

PhysicShape::PhysicShape(PhysicObject* owner, const std::vector<glm::vec3>& vertices)
	:	_owner{ owner }
{
	assert(owner);

	_vertices = vertices;

	/* calculate edges and the faces normals. We assume we have triangles. */
	for (size_t i = 0; i < _vertices.size();) {
		glm::vec3 v1 = _vertices[i++];
		glm::vec3 v2 = _vertices[i++];
		glm::vec3 v3 = _vertices[i++];

		glm::vec3 e1 = v2 - v1;
		glm::vec3 e2 = v3 - v2;
		glm::vec3 e3 = v1 - v3;

		_edges.push_back(e1);
		_edges.push_back(e2);
		_edges.push_back(e3);

		/* only take genuine normals: */
		bool takeIt = true;
		glm::vec3 normal = glm::normalize(glm::cross(e1, e2));
		for (size_t j = 0; j < _normals.size(); j++) {
			if (glm::dot(_normals[j], normal) > 1.0f - 0.001f) takeIt = false;
		}
		if (takeIt)
			_normals.push_back(normal);
	}

	std::cout << "NORMALS: " << _normals.size() << std::endl;
	for (int i = 0; i < _normals.size(); i++) {
		std::cout << "Normal[" << i << "]: " << _normals[i].x << "/" << _normals[i].y << "/" << _normals[i].z << std::endl;
	}
}


PhysicShape::~PhysicShape() {
}

void PhysicShape::normals(std::vector<glm::vec3>& axes) {
	glm::mat3 M = glm::toMat3(_owner->orientation());
	for (size_t i = 0; i < _normals.size(); i++) {
		axes.push_back(M * _normals[i]);
	}
}

void PhysicShape::minMaxOnAxis(const glm::vec3 & axis, glm::vec3 & outMin, glm::vec3 & outMax) const {
	
	/* we need to transform the axis to local space: */
	glm::mat4 W = _owner->transform()->worldMatrix();
	W = glm::scale(W, glm::vec3{ 0.5f, 0.5f, 0.5f });

	glm::mat3 iW = glm::transpose(glm::mat3(W));
	glm::vec3 localAxis = iW * axis;

	/* start finding min max values of the vertices projected on our local axis: */
	float projection;

	int minVertex, maxVertex;
	
	float minProjection = FLT_MAX;
	float maxProjection = -FLT_MAX;

	for (size_t i = 0; i < _vertices.size(); i++) {
		projection = glm::dot(localAxis, _vertices[i]);

		if (projection > maxProjection) {
			maxProjection = projection;
			maxVertex = i;
		}
		if (projection < minProjection) {
			minProjection = projection;
			minVertex = i;
		}
	}
	
	/* End of finding min max values -> prepare output: */
	outMin = glm::mat3(W) * _vertices[minVertex];
	outMax = glm::mat3(W) * _vertices[maxVertex];
}
}