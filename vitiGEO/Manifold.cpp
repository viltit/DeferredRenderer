#include "Manifold.hpp"

#include "PhysicObject.hpp"
#include "Constraint.hpp"

namespace vitiGEO {

Manifold::Manifold(PhysicObject * A, PhysicObject * B)
	:	_objA{ A },
		_objB{ B }
{}

Manifold::~Manifold() {
}

void Manifold::addContact(const glm::vec3 & globalOnA, const glm::vec3 & globalOnB, const glm::vec3 normal, float depth) {
	glm::vec3 r1 = globalOnA - _objA->transform()->pos();
	glm::vec3 r2 = globalOnB - _objB->transform()->pos();

	Contact contact { r1, r2, normal, depth };
	_contacts.push_back(contact);
}

void Manifold::applyImpulse() {
}

void Manifold::preSolver(float deltaTime) {
}

void Manifold::debugDraw() const {
}

void Manifold::updateConstraint(Contact & c) {
}

}