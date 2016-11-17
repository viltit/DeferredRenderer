#include "PhysicShape.hpp"

#include "PhysicObject.hpp"
#include <assert.h>

namespace vitiGEO {

PhysicShape::PhysicShape(PhysicObject* owner, const std::vector<glm::vec3>& vertices)
	:	_owner{ owner }
{
	assert(owner);

	/*	calculate vertices, edges and the faces normals. 
		this is a brute-force solution					*/
	
	/* vertices */
	for (size_t i = 0; i < vertices.size(); i++) {
		bool takeIt = true;
		for (int j = 0; j < _vertices.size(); j++) {
			if (vertices[i] == _vertices[j]) takeIt = false;
		}
		if (takeIt) _vertices.push_back(vertices[i]);
	}

	/* edges */
	for (size_t i = 0; i < _vertices.size();) {
		int edgeIndex1, edgeIndex2;
		edgeIndex1 = i;
		glm::vec3 v1 = _vertices[i++];
		edgeIndex2 = i;
		glm::vec3 v2 = _vertices[(i < _vertices.size())? i : 0];

		glm::vec3 e1 = v2 - v1;

		_edges.push_back(Edge{ edgeIndex2, edgeIndex1 });
	}

	/* normals: */
	for (size_t i = 0; i < vertices.size();) {
		glm::vec3 v1 = vertices[i++];
		glm::vec3 v2 = vertices[i++];
		glm::vec3 v3 = vertices[i++];

		glm::vec3 e1 = v2 - v1;
		glm::vec3 e2 = v3 - v2;

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
	std::cout << "Number of edges: " << _edges.size() << std::endl;
	std::cout << "Number of vertices: " << _vertices.size() << std::endl;

}


PhysicShape::~PhysicShape() {
}

void PhysicShape::normals(std::vector<glm::vec3>& axes) {
	glm::mat3 M = glm::toMat3(_owner->orientation());
	for (size_t i = 0; i < _normals.size(); i++) {
		axes.push_back(M * _normals[i]);
	}
}

void PhysicShape::edges(std::vector<glm::vec3>& edges) {
	glm::mat4 M4 = glm::toMat4(_owner->orientation());
	M4 = glm::scale(M4, glm::vec3{ 0.5f, 0.5f, 0.5f });
	glm::mat3 M = glm::mat3(M4);

	for (size_t i = 0; i < _edges.size(); i++) {
		glm::vec3 a = M * _vertices[_edges[i].eBegin];
		glm::vec3 b = M * _vertices[_edges[i].eEnd];
		edges.push_back(glm::vec3{ b - a });
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

	int minVertex{ 0 }, maxVertex{ 0 };
	
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