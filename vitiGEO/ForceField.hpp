/*	very simple spherical force field with linear interpolation 
	which I use for lifting objects in the scene around			*/

#pragma once


namespace vitiGEO {

class PhysicObject;

class ForceField {
public:
	ForceField(const PhysicObject* emitter, float maxForce, float radius, bool isActive = true);
	~ForceField();

	void update();

	void on()		{ _isActive = true; }
	void off()		{ _isActive = false; }

protected:
	const PhysicObject*	_emitter;
	
	float			_maxForce;
	float			_radius;

	bool			_isActive;
};
}
