#include "ForceField.hpp"

#include "PhysicObject.hpp"

#include <vector>

namespace vitiGEO {

ForceField::ForceField(const PhysicObject * emitter, float maxForce, float radius, bool isActive) 
	:	_emitter	{ emitter },
		_maxForce	{ maxForce },
		_radius		{ radius },
		_isActive	{ isActive }
{
	assert(_emitter);
}

ForceField::~ForceField() {
}

void update() {
	/* get all physic objects within the radius */
	std::vector<PhysicObject*> objsInRange;
	


	/* apply a force to these objects depending on their distance to the emitter: */

}

}