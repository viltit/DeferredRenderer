/*	very simple spherical force field with linear interpolation 
	which I use for lifting objects in the scene around			*/

#pragma once

#include <vector>

namespace vitiGEO {

class PhysicObject;
class Constraint;

class ForceField {
public:
	ForceField(PhysicObject* emitter, float maxForce, float radius, bool isActive = false);
	~ForceField();

	void on();
	void off();

	void setExclude(const std::vector<PhysicObject*> exclude) { _exclude = exclude; _exclude.push_back(_emitter); }

	bool isActive() { return _isActive; }

protected:
	PhysicObject*	_emitter;
	std::vector<PhysicObject*> _exclude;

	Constraint*		_constraint;

	float			_radius;

	bool			_isActive;
};
}
