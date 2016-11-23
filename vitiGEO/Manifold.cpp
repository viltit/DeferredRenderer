#include "Manifold.hpp"

#include "PhysicObject.hpp"
#include "PhysicEngine.hpp"

#include "DebugInfo.hpp"

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
	float softness = (_objA->invMass() + _objB->invMass()) / _contacts.size();

	for (Contact& contact : _contacts) {
		contact.normal.setSoftness(softness);
		contact.normal.addImpulse();

		float frictionLimit = contact.normal.iSum();
		contact.frictionA.setISumMin(-frictionLimit);
		contact.frictionA.setISumMax(frictionLimit);
		contact.frictionB.setISumMin(-frictionLimit);
		contact.frictionB.setISumMax(frictionLimit);

		contact.frictionA.addImpulse();
		contact.frictionB.addImpulse();
	}

}

void Manifold::preSolver(float deltaTime) {
	for (Contact& contact : _contacts)
		updateConstraint(contact);
}

void Manifold::debugDraw() const {
	if (_contacts.size() == 0) return;

	const Contact& c = _contacts.back();

	glm::vec3 globalOnA1 = _objA->transform()->pos() + _contacts.back().relPosA;
	for (const Contact& c : _contacts) {
		glm::vec3 globalOnA2 = _objA->transform()->pos() + c.relPosA;
		glm::vec3 globalOnB = _objB->transform()->pos() + c.relPosB;

		DebugInfo::instance()->addLine(glm::vec4{ globalOnA1, 0.1f }, glm::vec4{ globalOnA2, 0.1f });
		DebugInfo::instance()->addPoint(glm::vec4{ globalOnA2, 0.2f });
		DebugInfo::instance()->addLine(glm::vec4{ globalOnB, 0.1f }, glm::vec4{ globalOnA2, 0.1f });
		globalOnA1 = globalOnA2;
	}
}

void Manifold::updateConstraint(Contact & c) {
	glm::vec3 r1 = c.relPosA;
	glm::vec3 r2 = c.relPosB;

	glm::vec3 v1 = _objA->velocity() + glm::cross(_objA->angularVelocity(), r1);
	glm::vec3 v2 = _objB->velocity() + glm::cross(_objB->angularVelocity(), r2);

	glm::vec3 deltaV = v2 - v1;
	glm::vec3 tangent1 = deltaV - (c.cNormal * glm::dot(deltaV, c.cNormal));

	if (glm::dot(tangent1, tangent1) < 0.001f) {
		tangent1 = glm::cross(c.cNormal, glm::vec3{ 1.0f, 0.0f, 0.0f });
		if (glm::dot(tangent1, tangent1) < 0.001f) {
			tangent1 = glm::cross(c.cNormal, glm::vec3{ 0.0f, 0.0f, 1.0f });
		}
	}

	tangent1 = glm::normalize(tangent1);

	glm::vec3 tangent2 = glm::cross(c.cNormal, tangent1);
	tangent2 = glm::normalize(tangent2);

	/* normal collision constraint */
	float b = 0.0f;

	const float baumgarte_scalar = 0.2f;
	const float baumgarte_slop = 0.01f;
	float penetration_slop = fmin(c.depth + baumgarte_slop, 0.0f);
	b += (baumgarte_scalar / PhysicEngine::instance()->timestep() * penetration_slop);

	/* Elasticity */
	const float elasticity = 0.8f;
	const float elasticity_slop = 0.5f;

	float elasticity_term = elasticity * glm::dot(c.cNormal,
			- _objA->velocity()
			- glm::cross(r1, _objA->angularVelocity())
			+ _objB->velocity()
			+ glm::cross(r2, _objB->angularVelocity())
		);

	b += fmin(elasticity_term + elasticity_slop, 0.0f);

	/* Friction: */
	float friction = _objA->friction() * _objB->friction();

	/* and FINALLY, we are abe to set our constraints: */
	c.normal = Constraint{
		_objA, _objB,
		-c.cNormal,
		glm::cross(-r1, c.cNormal),
		c.cNormal,
		glm::cross(r2, c.cNormal),
		b };

	c.frictionA = Constraint{
		_objA, _objB,
		-tangent1 * friction,
		glm::cross(-r1, tangent1) * friction,
		tangent1 * friction,
		glm::cross(r2, tangent1) * friction,
		0.0f };

	c.frictionB = Constraint{
		_objA, _objB,
		-tangent2 * friction,
		glm::cross(-r1, tangent2) * friction,
		tangent2 * friction,
		glm::cross(r2, tangent2) * friction,
		0.0f };
}

}