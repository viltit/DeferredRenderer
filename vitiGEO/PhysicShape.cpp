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
	glm::mat4 M = glm::toMat4(_owner->orientation());
	M = glm::scale(M, _halfSize);

	for (size_t i = 0; i < _hull.numEdges(); ++i) {
		const hEdge& edge = _hull.edge(i);
		glm::vec3 a = M *  glm::vec4{ _hull.vertex(edge.vStart).pos, 1.0f };
		glm::vec3 b = M * glm::vec4{ _hull.vertex(edge.vEnd).pos, 1.0f };
		edges.push_back(glm::vec3{ b - a });
	}
}

void CuboidShape::minMaxOnAxis(const glm::vec3 & axis, glm::vec3 & outMin, glm::vec3 & outMax) const {
	
	/* we need to transform the axis to local space: */
	glm::mat4 W = _owner->transform()->worldMatrix();
	W = glm::scale(W, _halfSize);

	glm::mat4 iW = glm::transpose((W));
	glm::vec3 localAxis = iW * glm::vec4{ axis, 1.0f };

	int minID, maxID;
	_hull.minMaxOnAxis(localAxis, minID, maxID);

	outMin = W * glm::vec4{	_hull.vertex(minID).pos, 1.0f };
	outMax = W * glm::vec4{ _hull.vertex(maxID).pos, 1.0f };
}

void CuboidShape::calcIncidentReference(const glm::vec3 & axis, std::list<glm::vec3>& outFace,
	glm::vec3& outNormal, std::vector<Plane>& outAdjacentPlanes) 
{
	glm::mat4 W = _owner->transform()->worldMatrix();
	W = glm::scale(W, _halfSize);

	glm::mat3 iW = glm::inverse(glm::mat3(W));
	glm::mat3 tW = glm::transpose(iW);

	glm::vec3 localAxis = iW * axis;

	/* get min max vertex along localized axis: */
	int minVertex;
	int maxVertex;
	_hull.minMaxOnAxis(localAxis, minVertex, maxVertex);
	
	/* check which face is closest to parallel to local axis: */
	const hVertex& vertex = _hull.vertex(maxVertex);
	const hFace* bestFace = nullptr;
	float bestCorrelation = -FLT_MAX;

	for (int faceID : vertex.faces) {
		const hFace* face = &_hull.face(faceID);
		float correlation = glm::dot(localAxis, face->normal);
		if (correlation > bestCorrelation) {
			bestCorrelation = correlation;
			bestFace = face;
		}
	}

	if (!bestFace) return;

	/* prepare the functions output: */
	outNormal = glm::normalize(tW * bestFace->normal);
	for (int vertexID : bestFace->vertices) {
		glm::vec3 vertex = _hull.vertex(vertexID).pos;
		outFace.push_back(W * glm::vec4{ vertex, 1.0f });
	}

	/* add reference face to the list of adjacent planes: */
	glm::vec3 pointOnPlane = W * glm::vec4{ _hull.vertex(_hull.edge(bestFace->edges[0]).vStart).pos, 1.0f };
	glm::vec3 planeNormal = -(tW * glm::vec4{ bestFace->normal, 1.0f });
	planeNormal = glm::normalize(planeNormal);
	float planeDistance = -glm::dot(planeNormal, pointOnPlane);

	outAdjacentPlanes.push_back(Plane{ planeNormal, planeDistance, false });

	/* get all edges of the bestFace and then extract the adjacent faces via these edges: */
	for (int edgeID : bestFace->edges) {
		const hEdge& edge = _hull.edge(edgeID);
		pointOnPlane = W * glm::vec4{ _hull.vertex(edge.vStart).pos, 1.0f };

		for (int faceID : edge.faces) {
			if (faceID != bestFace->id) {
				const hFace& adjacentFace = _hull.face(faceID);

				planeNormal = -(tW * glm::vec4{ adjacentFace.normal, 1.0f });
				planeNormal = glm::normalize(planeNormal);
				planeDistance = -glm::dot(planeNormal, pointOnPlane);

				outAdjacentPlanes.push_back(Plane{ planeNormal, planeDistance, false });
			}
		}
	}
}


void CuboidShape::debugDraw() {
	glm::mat4 M = _owner->transform()->worldMatrix();
	M = glm::scale(M, _halfSize);
	_hull.drawDebug(M);
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