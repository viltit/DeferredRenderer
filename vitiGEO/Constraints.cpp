#include "Constraints.hpp"

#include "PhysicObject.hpp"
#include "Transform.hpp"
#include "Physics.hpp"

namespace vitiGEO {


void Constraint::remove() {
	if (_constraint) {
		Physics::instance()->removeConstraint(this);
		delete _constraint;
		_constraint = nullptr;
	}
}

/* Point to Point -------------------------------------------------------- */
P2PConstraint::P2PConstraint(const PhysicObject * objA, const glm::vec3 & pivotA, 
							 const PhysicObject * objB, const glm::vec3 & pivotB) 
{
	assert(objA);

	btVector3 pointOnA = glmVecToBtVec(pivotA);
	btVector3 pointOnB = glmVecToBtVec(pivotB);

	if (objA && !objB) {
		_constraint = new btPoint2PointConstraint(*objA->body(), pointOnA);
		Physics::instance()->addConstraint(this);
	}

	else if (objA && objB) {
		_constraint = new btPoint2PointConstraint(*objA->body(), *objB->body(), pointOnA, pointOnB);
		Physics::instance()->addConstraint(this);
	}
}

P2PConstraint::~P2PConstraint() {
	remove();
}


/* Slider ----------------------------------------------------------------- */
SliderConstraint::SliderConstraint(const PhysicObject * objA, const PhysicObject * objB,
								   float minDist, float maxDist) {
	btTransform inA{ btTransform::getIdentity() };
	btTransform inB{ btTransform::getIdentity() };
	
	inA.setOrigin(btVector3{ 0.0f, 0.0f, 0.0f });
	inB.setOrigin(btVector3{ -1.f, 0.0f, -1.0f });
	inA.getBasis().setEulerZYX(0.f, 0.f, 0);
	inB.getBasis().setEulerZYX(M_PI * 0.5, 0.f, 0.f);

	_constraint = new btSliderConstraint{ *objA->body(), *objB->body(), inA, inB, true };
	btSliderConstraint* c = static_cast<btSliderConstraint*>(_constraint);

	Physics::instance()->addConstraint(this);

	setMinMaxLinear(minDist, maxDist);
}

SliderConstraint::SliderConstraint(const PhysicObject * objA, float minDist, float maxDist) {
	btTransform inA{ btTransform::getIdentity() };
	inA.setOrigin(btVector3{ 30.0f, 0.5f, 30.0f });

	_constraint = new btSliderConstraint{ *objA->body(), inA, false };

	Physics::instance()->addConstraint(this);

	setMinMaxLinear(minDist, maxDist);
}

SliderConstraint::~SliderConstraint() {
	remove();
}

void SliderConstraint::setMinMaxAngle(float minAngle, float maxAngle) {
	btSliderConstraint* c = static_cast<btSliderConstraint*>(_constraint);
	c->setLowerAngLimit(minAngle);
	c->setUpperAngLimit(maxAngle);
}

void SliderConstraint::setMinMaxLinear(float min, float max) {
	btSliderConstraint* c = static_cast<btSliderConstraint*>(_constraint);
	c->setLowerLinLimit(min);
	c->setUpperLinLimit(max);
}

void SliderConstraint::addMotor(float desiredVelocity, float maxForce) {
	static_cast<btSliderConstraint*>(_constraint)->setTargetLinMotorVelocity(desiredVelocity);
	static_cast<btSliderConstraint*>(_constraint)->setMaxLinMotorForce(maxForce);
}

void SliderConstraint::motorOn() {
	static_cast<btSliderConstraint*>(_constraint)->setPoweredLinMotor(true);
}

void SliderConstraint::motorOff() {
	static_cast<btSliderConstraint*>(_constraint)->setPoweredLinMotor(true);
}


/* Hinge ----------------------------------------------------------------- */
HingeConstraint::HingeConstraint(const PhysicObject * obj, const glm::vec3 pivot, const glm::vec3 axis) {
	btVector3 pivotA = glmVecToBtVec(pivot);
	btVector3 axisA = glmVecToBtVec(axis);
	_constraint = new btHingeConstraint{ *obj->body(), pivotA, axisA };

	Physics::instance()->addConstraint(this);
}

HingeConstraint::~HingeConstraint() {
}

void HingeConstraint::setMinMax(float min, float max) {
	btHingeConstraint* c = static_cast<btHingeConstraint*>(_constraint);
	c->setLimit(min, max);
}

void HingeConstraint::setSoftness(float soft) {
	btHingeConstraint* c = static_cast<btHingeConstraint*>(_constraint);
	c->setLimit(c->getLowerLimit(), c->getUpperLimit(), soft);
}

void HingeConstraint::addMotor(float targetVelocity, float maxImpulse) {
	static_cast<btHingeConstraint*>(_constraint)->enableAngularMotor(
		true, targetVelocity, maxImpulse
	);
}

void HingeConstraint::motorOff() {
	static_cast<btHingeConstraint*>(_constraint)->enableMotor(false);
}

void HingeConstraint::motorOn() {
	static_cast<btHingeConstraint*>(_constraint)->enableMotor(true);
}

/* Chain -------------------------------------------------------------------- */
Chain::Chain(const std::vector<PhysicObject*> objects, float distance) {
	assert(objects.size() > 1);

	for (size_t i = 0; i < objects.size() - 1; i++) {
		PhysicObject* objA = objects[i];
		PhysicObject* objB = objects[i + 1];

		/*	crude approximation of pivot point */
		glm::vec3 pivotInA = distance * glm::normalize(objB->transform()->pos() - objA->transform()->pos());
		glm::vec3 pivotInB = distance * glm::normalize(objA->transform()->pos() - objB->transform()->pos());

		P2PConstraint* p2p = new P2PConstraint{objA, pivotInA, objB, pivotInB };
		_p2p.push_back(p2p);
	}
}

Chain::~Chain() {
	for (auto& c : _p2p) {
		if (c == nullptr) {
			delete c;
			c = nullptr;
		}
	}
}

}