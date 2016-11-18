#include "Collidor.hpp"

#include "PhysicObject.hpp"
#include "PhysicShape.hpp"
#include "AABB.hpp"

namespace vitiGEO {

Collidor::Collidor(){
}

Collidor::~Collidor() {
}

bool Collidor::AABBIntersect(const PhysicObject * obj1, const PhysicObject * obj2) {
	if (AABBIntersection(obj1->aabb(), obj2->aabb())) return true;
	//return false;
	return true;
}

bool Collidor::SAT(const PhysicObject * obj1, const PhysicObject * obj2, CollidorData & out) {
	CollidorData currentCollision;
	CollidorData bestCollision;
	bestCollision.depth = -FLT_MAX;

	/* Assemble the axes we need to test against: */
	std::vector<glm::vec3> axes;
	obj1->shape()->collisionNormals(axes);
	obj2->shape()->collisionNormals(axes);

	/* Add edge-edge axes */
	std::vector<glm::vec3> edges1;
	std::vector<glm::vec3> edges2;
	obj1->shape()->edges(edges1);
	obj2->shape()->edges(edges2);

	for (auto& E1 : edges1) {
		for (auto& E2 : edges2) {
			E1 = glm::normalize(E1);
			E2 = glm::normalize(E2);
			addSATAxis(glm::cross(E1, E2), axes);
		}
	}

	/* test if we find a separating axis between the two objs and bail out if we do: */
	for (const auto& a : axes) {
		if (!testSATAxis(a, obj1, obj2, currentCollision)) {
			return false;
		}
		if (currentCollision.depth >= bestCollision.depth) {
			bestCollision = currentCollision;
		}
	}   
	out = bestCollision;
	
	//std::cout << "Depth: " << out.depth << std::endl;
	//std::cout << "Hit normal: " << out.hitNormal.x << "/" << out.hitNormal.y << "/" << out.hitNormal.z << std::endl;
	return true;
}

void Collidor::addSATAxis(const glm::vec3& axis, std::vector<glm::vec3>& SATAxes) {
	if (glm::dot(axis, axis) < EPSILON_TRESHOLD) return;

	for (const auto& a : SATAxes) {
		if (glm::dot(a, axis) > 1.0f - EPSILON_TRESHOLD) return;
	}

	SATAxes.push_back(axis);
}
	
bool Collidor::testSATAxis(const glm::vec3 axis, const PhysicObject * obj1, const PhysicObject * obj2, CollidorData & out) const {
	glm::vec3 min1, min2, max1, max2;

	obj1->shape()->minMaxOnAxis(axis, min1, max1);
	obj2->shape()->minMaxOnAxis(axis, min2, max2);

	float minProjection1 = glm::dot(axis, min1);
	float maxProjection1 = glm::dot(axis, max1);
	float minProjection2 = glm::dot(axis, min2);
	float maxProjection2 = glm::dot(axis, max2);

	if (minProjection1 <= minProjection2 && maxProjection1 >= minProjection2) {
		out.hitNormal = axis;
		out.depth = minProjection2 - maxProjection1;
		out.hitPoint = max1 + out.hitNormal * out.depth;
		return true;
	}

	if (minProjection2 <= minProjection1 && maxProjection2 > minProjection1) {
		out.hitNormal = -axis;
		out.depth = minProjection1 - maxProjection2;
		out.hitPoint = min1 + out.hitNormal * out.depth;
		return true;
	}

	return false;
}

}