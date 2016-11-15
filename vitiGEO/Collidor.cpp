#include "Collidor.hpp"

#include "PhysicObject.hpp"
#include "AABB.hpp"

namespace vitiGEO {

Collidor::Collidor(){
}

Collidor::~Collidor() {
}

bool Collidor::AABBIntersect(const PhysicObject * obj1, const PhysicObject * obj2) {
	if (AABBIntersection(obj1->aabb(), obj2->aabb())) return true;
	return false;
}

bool Collidor::SAT(const PhysicObject * obj1, const PhysicObject * obj2, CollidorData & out)
{
	return false;
}

void Collidor::addSATAxis(const glm::vec3& axis, std::vector<glm::vec3>& SATAxes) {
	if (glm::dot(axis, axis) < EPSILON_TRESHOLD) return;

	for (const auto& a : SATAxes) {
		if (glm::dot(a, axis) > 1.0f - EPSILON_TRESHOLD) return;
	}

	SATAxes.push_back(axis);
}

}