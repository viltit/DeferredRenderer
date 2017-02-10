#include "ForceField.hpp"

#include "PhysicObject.hpp"
#include "Constraints.hpp"
#include "Physics.hpp"

#include <vector>

namespace vitiGEO {

ForceField::ForceField(PhysicObject * emitter, float maxForce, float radius, bool isActive) 
	:	_emitter	{ emitter },
		_radius		{ radius },
		_isActive	{ isActive },
		_constraint { nullptr }
{
	assert(_emitter);
}

ForceField::~ForceField() {
	off();
}

void ForceField::on() {
	std::cout << "ForceField on\n";
	std::vector<PhysicObject*> objs = Physics::instance()->objects();
	PhysicObject* body{ nullptr};

	btVector3 center = _emitter->body()->getWorldTransform().getOrigin();

	float closest = 2.f * _radius;

	/* find the clostest object in range: */
	for (const auto& o : objs) {
		btVector3 pos = o->body()->getWorldTransform().getOrigin();
		float distance = (center - pos).length();
		if (distance < _radius && distance < closest) {
			/* check if the body in question is excluded: */
			bool doIt = true;
			for (auto& e : _exclude) {
				if (e == o) doIt = false;
			}
			if (doIt) {
				closest = distance;
				body = o;
				std::cout << "<FORCE_FIELD>Found an object\n";
			}
		}
	}

	/* if we have a clostest obj in range, apply a constraint: */
	if (body) {
		btVector3 max;
		btVector3 min;

		/*	hacky way to get pivot points
			more precise: shoot a ray from body to emitter and use rays hit point
		*/

		body->body()->getAabb(min, max);
		float sizeA = (max - min).length() / 2.f;
		glm::vec3 pivotA = body->transform()->pos() + glm::vec3{ 0.f, sizeA, 0.f };

		_emitter->body()->getAabb(min, max);
		float sizeB = (max - min).length() / 2.f;
		glm::vec3 pivotB = _emitter->transform()->pos() - glm::vec3{ 0.f, sizeB, 0.f };

		_constraint = new P2PConstraint{ body, pivotA, _emitter, pivotB };
	}

	_isActive = true;
}

void ForceField::off() {
	if (_constraint) {
		_constraint->remove();
		delete _constraint;
		_constraint = nullptr;
		_isActive = false;
	}
}

}