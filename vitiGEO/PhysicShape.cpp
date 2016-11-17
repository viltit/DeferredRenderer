#include "PhysicShape.hpp"

#include "PhysicObject.hpp"
#include <assert.h>

namespace vitiGEO {



CuboidShape::CuboidShape(PhysicObject* owner)
	:	IPhysicShape{ owner }
{
	assert(owner);

	/*	hard coded vertices, edges and normals: 
		(we only store normals and edges relevant for collision testing) */
	_vertices.push_back(glm::vec3{ -1.0f, -1.0f, -1.0f });
	_vertices.push_back(glm::vec3{ -1.0f, 1.0f, -1.0f });
	_vertices.push_back(glm::vec3{ 1.0f, 1.0f, -1.0f });
	_vertices.push_back(glm::vec3{ 1.0f, -1.0f, -1.0f });

	_vertices.push_back(glm::vec3{ -1.0f, -1.0f, 1.0f });
	_vertices.push_back(glm::vec3{ -1.0f, 1.0f, 1.0f });
	_vertices.push_back(glm::vec3{ 1.0f, 1.0f, 1.0f });
	_vertices.push_back(glm::vec3{ 1.0f, -1.0f, 1.0f });

	_normals.push_back(glm::vec3{ 1.0f, 0.0f, 0.0f });
	_normals.push_back(glm::vec3{ 0.0f, 1.0f, 0.0f });
	_normals.push_back(glm::vec3{ 0.0f, 0.0f, 1.0f });

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
void CuboidShape::normals(std::vector<glm::vec3>& axes)
{
}
void CuboidShape::edges(std::vector<glm::vec3>& edges)
{
}
void CuboidShape::minMaxOnAxis(const glm::vec3 & axis, glm::vec3 & outMin, glm::vec3 & outMax) const
{
}
}