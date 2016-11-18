#include "PhysicShape.hpp"

#include "PhysicObject.hpp"
#include <assert.h>

namespace vitiGEO {

Hull CuboidShape::_hull;

CuboidShape::CuboidShape(PhysicObject* owner, const glm::vec3& halfSize)
	:	IPhysicShape{ owner },
		_halfSize{ halfSize }
{
	assert(owner);
	if (_hull.numVertices() == 0)
		initHull();
}

CuboidShape::~CuboidShape() {
}

glm::mat3 CuboidShape::inverseInertia(float invMass) const {
	glm::mat3 inertia{ };
	glm::vec3 sizeSq{ 2.0f * _halfSize };
	sizeSq = sizeSq * sizeSq;

	inertia[0][0] = 12.f * invMass / (sizeSq.y + sizeSq.z);
	inertia[1][1] = 12.f * invMass / (sizeSq.x + sizeSq.z);
	inertia[2][2] = 12.f * invMass / (sizeSq.x + sizeSq.y);

	return inertia;
}

void CuboidShape::collisionNormals(std::vector<glm::vec3>& axes) const {
	glm::mat3 M = glm::toMat3(_owner->orientation());
	axes.push_back(M * glm::vec3{ 1.0f, 0.0f, 0.0f });
	axes.push_back(M * glm::vec3{ 0.0f, 1.0f, 0.0f });
	axes.push_back(M * glm::vec3{ 0.0f, 0.0f, 1.0f });
}


void CuboidShape::edges(std::vector<glm::vec3>& edges) const {
	glm::mat4 M4 = glm::toMat4(_owner->orientation());
	M4 = glm::scale(M4, _halfSize);
	glm::mat3 M = glm::mat3(M4);

	for (size_t i = 0; i < _hull.numEdges(); ++i) {
		const hEdge& edge = _hull.edge(i);
		glm::vec3 a = M *  _hull.vertex(edge.vStart).pos;
		glm::vec3 b = M * _hull.vertex(edge.vEnd).pos;
		edges.push_back(glm::vec3{ b - a });
	}
}

void CuboidShape::minMaxOnAxis(const glm::vec3 & axis, glm::vec3 & outMin, glm::vec3 & outMax) const {
	
	/* we need to transform the axis to local space: */
	glm::mat4 W = _owner->transform()->worldMatrix();
	W = glm::scale(W, _halfSize);

	glm::mat3 iW = glm::transpose(glm::mat3(W));
	glm::vec3 localAxis = iW * axis;

	/* TO DO: call Hull::minMaxOnAxis()*/
	_hull.minMaxOnAxis(localAxis, outMin, outMax);

	outMin = glm::mat3(W) * outMin;
	outMax = glm::mat3(W) * outMax;
}


void CuboidShape::initHull() {
	_hull.addVertex(glm::vec3(-1.0f, -1.0f, -1.0f));	// 0
	_hull.addVertex(glm::vec3(-1.0f, 1.0f, -1.0f));		// 1
	_hull.addVertex(glm::vec3(1.0f, 1.0f, -1.0f));		// 2
	_hull.addVertex(glm::vec3(1.0f, -1.0f, -1.0f));		// 3

	_hull.addVertex(glm::vec3(-1.0f, -1.0f, 1.0f));		// 4
	_hull.addVertex(glm::vec3(-1.0f, 1.0f, 1.0f));		// 5
	_hull.addVertex(glm::vec3(1.0f, 1.0f, 1.0f));		// 6
	_hull.addVertex(glm::vec3(1.0f, -1.0f, 1.0f));		// 7

	std::vector<int> face1 = { 0, 1, 2, 3 };
	std::vector<int> face2 = { 7, 6, 5, 4 };
	std::vector<int> face3 = { 5, 6, 2, 1 };
	std::vector<int> face4 = { 0, 3, 7, 4 };
	std::vector<int> face5 = { 6, 7, 3, 2 };
	std::vector<int> face6 = { 4, 5, 1, 0 };

	_hull.addFace(glm::vec3(0.0f, 0.0f, -1.0f), face1);
	_hull.addFace(glm::vec3(0.0f, 0.0f, 1.0f), face2);
	_hull.addFace(glm::vec3(0.0f, 1.0f, 0.0f), face3);
	_hull.addFace(glm::vec3(0.0f, -1.0f, 0.0f), face4);
	_hull.addFace(glm::vec3(1.0f, 0.0f, 0.0f), face5);
	_hull.addFace(glm::vec3(-1.0f, 0.0f, 0.0f), face6);
}
}