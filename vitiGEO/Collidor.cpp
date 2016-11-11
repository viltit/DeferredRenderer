#include "Collidor.hpp"

namespace vitiGEO {

Collidor::Collidor()
{
}


Collidor::~Collidor()
{
}

bool Collidor::AABBIntersection(const PhysicObject * obj1, const PhysicObject * obj2)
{
	return false;
}

bool Collidor::SAT(const PhysicObject * obj1, const PhysicObject * obj2, CollidorData & out)
{
	return false;
}

}