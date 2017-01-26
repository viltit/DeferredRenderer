/* Simple Wrapper for easier access to btConstraints */

#pragma once

#include <bt/btBulletCollisionCommon.h>
#include <bt/btBulletDynamicsCommon.h>

#include <glm/glm.hpp>

#include <vector>

namespace vitiGEO {

class PhysicObject;

/* abstract base class -------------------------------------------------- */
class Constraint {
public:
	Constraint();
	virtual ~Constraint() {};

	void remove();

	void setBreakingImpulse(float force) { _constraint->setBreakingImpulseThreshold(force); }

	/* access btConstraint: */
	btTypedConstraint* obj() { return _constraint; }

protected:
	btTypedConstraint* _constraint;

	const PhysicObject*		_objA;
	const PhysicObject*		_objB;
};


/* Point to Point -------------------------------------------------------- */
class P2PConstraint : public Constraint {
public:
	P2PConstraint(const PhysicObject* objA, const glm::vec3& pivotA, 
				  const PhysicObject* objB = nullptr, const glm::vec3& pivotB = {});
	~P2PConstraint();

private:
};

/* Slider ----------------------------------------------------------------- */
class SliderConstraint : public Constraint {
public:
	SliderConstraint(const PhysicObject* objA, const glm::vec3& pivotInA,
					 const PhysicObject* objB, const glm::vec3& pivotInB,
					 float minDist = 0.0f, float maxDist = 10.0f);

	/* this constructor does not work properly yet: */
	SliderConstraint(const PhysicObject* objA, float minDist = 0.0f, float maxDist = 10.0f);

	~SliderConstraint();

	void setMinMaxAngle(float minAngle, float maxAngle);
	void setMinMaxLinear(float min, float max);

	void addMotor(float desiredVelocity, float maxForce);

	void motorOn();
	void motorOff();

private:
};


/* Hinge ----------------------------------------------------------------- */
class HingeConstraint : public Constraint {
public:
	HingeConstraint(const PhysicObject* obj, const glm::vec3 pivot, const glm::vec3 axis);
	~HingeConstraint();

	void setMinMax(float min, float max);
	void setSoftness(float soft);

	void addMotor(float targetVelocity, float maxImpulse);
	void motorOff();
	void motorOn();

private:

};

/* Chain -------------------------------------------------------------------- */
class Chain {
public:
	Chain(const std::vector<PhysicObject*> objects, float distance);
	~Chain();

protected:
	std::vector<P2PConstraint*> _p2p;
};

/* Chain  with two P2P Constraints each ------------------------------------- */
class CuboidChain {
public:
	CuboidChain(const std::vector<PhysicObject*> objects, float distance);
	~CuboidChain();

	/* getters and setters: */
	PhysicObject* first() const		{ return _objects[0]; }
	PhysicObject* last() const		{ return _objects[_objects.size() - 1]; }

protected:
	std::vector<PhysicObject*>	_objects;
	std::vector<P2PConstraint*> _p2p;

};

}

