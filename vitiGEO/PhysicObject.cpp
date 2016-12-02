#include "PhysicObject.hpp"

#include "Physics.hpp"

namespace vitiGEO {

PhysicObject::PhysicObject(Transform* transform)
	: _transform{ transform }
{}

PhysicObject::~PhysicObject() {
}

void PhysicObject::remove() {
	Physics::instance()->removeObject(this);
}


/* Cuboid ------------------------------------------------------------------------------------ */
CuboidObject::CuboidObject(Transform * transform, 
	const void* node,
	float mass,
	const glm::vec3& dimensions,
	const glm::vec3 & initVelocity)

	: PhysicObject{ transform }
{
	/* adapt orientation and position: */
	btTransform t;

	t.setIdentity();
	t.setOrigin(glmVecToBtVec(transform->pos()));
	t.setRotation(glmQuatToBtQuat(transform->orientation()));

	btBoxShape* shape = new btBoxShape(glmVecToBtVec(_transform->scale() * dimensions / 2.0f));

	btVector3 inertia;
	shape->calculateLocalInertia(mass, inertia);

	btMotionState* motion = new btDefaultMotionState(t);

	/* create the rigid body: */
	_body = new btRigidBody{ btScalar(mass), motion, shape, inertia };

	/* set velocity: */
	_body->setLinearVelocity(glmVecToBtVec(initVelocity));
	_body->setLinearFactor(btVector3{ 0.8f, 0.8f, 0.8f });
	_body->setCollisionFlags(_body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	_body->setUserPointer((void*)node);

	Physics::instance()->addObject(this);
}

CuboidObject::~CuboidObject() {
	if (_body) {
		delete _body;
		_body = nullptr;
	}
}

void CuboidObject::update() {
	btTransform t;
	_body->getMotionState()->getWorldTransform(t);

	/* adapt position: */
	glm::vec3 pos = btVecToGlmVec(_body->getCenterOfMassPosition());
	_transform->setPos(pos);

	/* adapt orientation: */
	//glm::quat o1 = _transform->orientation();
	glm::quat o2 = btQuatToGlmQuat(t.getRotation());
	_transform->rotateTo(o2 );
}

/* Plane ------------------------------------------------------------------------------------ */
PlaneObject::PlaneObject(Transform * transform,
	const void* node,
	float d,
	const glm::vec3 & normal)

	: PhysicObject{ transform }
{
	/* adapt orientation and position: */
	btTransform t;

	t.setIdentity();

	//t.setOrigin(glmVecToBtVec(transform->pos()));
	t.setRotation(glmQuatToBtQuat(transform->orientation()));

	btStaticPlaneShape* shape = new btStaticPlaneShape(glmVecToBtVec(normal), d);
	btMotionState* motion = new btDefaultMotionState(t);

	/* create the rigid body: */
	_body = new btRigidBody{ btScalar(0.0f), motion, shape };


	Physics::instance()->addObject(this);
}

PlaneObject::~PlaneObject() {
	if (_body) {
		delete _body;
		_body = nullptr;
	}
}

void PlaneObject::update() {
}
}