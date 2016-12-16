#include "PhysicObject.hpp"

#include "Physics.hpp"
#include "DebugInfo.hpp"

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

	_shape = new btBoxShape(glmVecToBtVec(_transform->scale() * dimensions / 2.0f));

	btVector3 inertia;
	_shape->calculateLocalInertia(mass, inertia);

	btMotionState* motion = new btDefaultMotionState(t);

	/* create the rigid body: */
	_body = new btRigidBody{ btScalar(mass), motion, _shape, inertia };

	/* set velocity: */
	_body->setLinearVelocity(glmVecToBtVec(initVelocity));
	_body->setLinearFactor(btVector3{ 0.8f, 0.8f, 0.8f });
	_body->setCollisionFlags(_body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	_body->setUserPointer((void*)node);

	Physics::instance()->addObject(this);
}

CuboidObject::~CuboidObject() {
	if (_body) {
		Physics::instance()->removeObject(this);
		delete _body;
		_body = nullptr;
	}
	if (_shape) {
		delete _shape;
		_shape = nullptr;
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

	_shape = new btStaticPlaneShape(glmVecToBtVec(normal), d);
	btMotionState* motion = new btDefaultMotionState(t);

	/* create the rigid body: */
	_body = new btRigidBody{ btScalar(0.0f), motion, _shape };

	Physics::instance()->addObject(this);
}

PlaneObject::~PlaneObject() {
	if (_body) {
		Physics::instance()->removeObject(this);
		delete _body;
		_body = nullptr;
	}
	if (_shape) {
		delete _shape;
		_shape = nullptr;
	}
}

void PlaneObject::update() {
	/* no update needed since its a static object */
}


/* */
ConvexHullObject::ConvexHullObject(
	Transform * transform, 
	const std::vector<glm::vec3>& points,
	const void * node, float mass, 
	const glm::vec3 & initialVelocity)
	
	: PhysicObject{ transform }
{
	/* adapt orientation and position: */
	btTransform t;

	t.setIdentity();
	t.setOrigin(glmVecToBtVec(transform->pos()));
	t.setRotation(glmQuatToBtQuat(transform->orientation()));

	/* transform glm vectors to bt scalars: */
	std::vector<btScalar> btPoints;
	for (const auto& point : points) {
		btPoints.push_back(point.x);
		btPoints.push_back(point.y);
		btPoints.push_back(point.z);
	}

	_shape = new btConvexHullShape(btPoints.data(), points.size(), 3 * sizeof(btScalar));

	btVector3 inertia;
	_shape->calculateLocalInertia(mass, inertia);

	btMotionState* motion = new btDefaultMotionState(t);

	/* create the rigid body: */
	_body = new btRigidBody{ btScalar(mass), motion, _shape, inertia };

	/* set velocity: */
	_body->setLinearVelocity(glmVecToBtVec(initialVelocity));
	_body->setLinearFactor(btVector3{ 0.8f, 0.8f, 0.8f });
	_body->setCollisionFlags(_body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	_body->setUserPointer((void*)node);

	Physics::instance()->addObject(this);
}

ConvexHullObject::~ConvexHullObject() {
	if (_body) {
		Physics::instance()->removeObject(this);
		delete _body;
		_body = nullptr;
	}
	if (_shape) {
		delete _shape;
		_shape = nullptr;
	}
}

void ConvexHullObject::update() {
	btTransform t;
	_body->getMotionState()->getWorldTransform(t);

	/* adapt position: */
	glm::vec3 pos = btVecToGlmVec(_body->getCenterOfMassPosition());
	_transform->setPos(pos);

	/* adapt orientation: */
	//glm::quat o1 = _transform->orientation();
	glm::quat o2 = btQuatToGlmQuat(t.getRotation());
	_transform->rotateTo(o2);
}

SphereObject::SphereObject(Transform * transform, const void * node, float mass, float radius, const glm::vec3& initialVelocity)
	: PhysicObject{ transform }
{
	/* adapt orientation and position: */
	btTransform t;

	t.setIdentity();
	t.setOrigin(glmVecToBtVec(transform->pos()));
	t.setRotation(glmQuatToBtQuat(transform->orientation()));

	/* we assume x y and z scale are identical since it is a sphere: */
	_shape = new btSphereShape(radius * _transform->scale().x);

	btVector3 inertia;
	_shape->calculateLocalInertia(mass, inertia);

	btMotionState* motion = new btDefaultMotionState(t);

	/* create the rigid body: */
	_body = new btRigidBody{ btScalar(mass), motion, _shape, inertia };

	/* set velocity: */
	_body->setLinearVelocity(glmVecToBtVec(initialVelocity));
	_body->setLinearFactor(btVector3{ 0.8f, 0.8f, 0.8f });
	_body->setCollisionFlags(_body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	_body->setUserPointer((void*)node);

	Physics::instance()->addObject(this);
}

SphereObject::~SphereObject() {
	if (_body) {
		Physics::instance()->removeObject(this);
		delete _body;
		_body = nullptr;
	}
	if (_shape) {
		delete _shape;
		_shape = nullptr;
	}
}

void SphereObject::update() {
	btTransform t;
	_body->getMotionState()->getWorldTransform(t);

	/* adapt position: */
	glm::vec3 pos = btVecToGlmVec(_body->getCenterOfMassPosition());
	_transform->setPos(pos);

	/* adapt orientation: */
	//glm::quat o1 = _transform->orientation();
	glm::quat o2 = btQuatToGlmQuat(t.getRotation());
	_transform->rotateTo(o2);
}

/*	MULIT-BODY --------------------------------------------------------------------- */

MultiBody::MultiBody(Transform* transform, const void* node, float mass)
	: PhysicObject{ transform }
{
	/* adapt orientation and position: */
	btTransform t;

	t.setIdentity();
	t.setOrigin(glmVecToBtVec(transform->pos()));
	t.setRotation(glmQuatToBtQuat(transform->orientation()));

	/* this is an empty shape for now: */
	_shape = new btCompoundShape();

	btVector3 inertia;
	_shape->calculateLocalInertia(mass, inertia);

	btMotionState* motion = new btDefaultMotionState(t);

	/* create the rigid body: */
	_body = new btRigidBody{ btScalar(mass), motion, _shape, inertia };

	_body->setLinearFactor(btVector3{ 0.8f, 0.8f, 0.8f });
	_body->setCollisionFlags(_body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	_body->setUserPointer((void*)node);

	Physics::instance()->addObject(this);
}

MultiBody::~MultiBody() {
}

void MultiBody::addCuboidBody(Transform* transform,
							float mass,
							const glm::vec3& dimensions) 
{
	_transforms.push_back(transform);

	/* adapt orientation and position: */
	btTransform t;

	t.setIdentity();
	t.setOrigin(glmVecToBtVec(transform->pos()));
	t.setRotation(glmQuatToBtQuat(transform->orientation()));

	btBoxShape* child = new btBoxShape(glmVecToBtVec(transform->scale() * dimensions / 2.0f));

	btVector3 inertia;
	child->calculateLocalInertia(mass, inertia);

	btMotionState* motion = new btDefaultMotionState(t);

	static_cast<btCompoundShape*>(_shape)->addChildShape(t, child);
}

void MultiBody::update() {

	size_t numChildren = static_cast<btCompoundShape*>(_shape)->getNumChildShapes();

	assert(_transforms.size() == numChildren);

	for (size_t i = 0; i < numChildren; i++) {
		btTransform t = static_cast<btCompoundShape*>(_shape)->getChildTransform(i);
		glm::vec3 pos = btVecToGlmVec(t.getOrigin() + _body->getCenterOfMassPosition());
		glm::quat o = btQuatToGlmQuat(t.getRotation());

		/* THIS IS WRONG 
			-> position is off
			-> rotation does not happen
		*/
		std::cout << "Multi-Object child[" << i << "] :\n";
		std::cout << "Pos: " << pos.x << "/" << pos.y << "/" << pos.z << std::endl;
		std::cout << "O: " << o.w << "/" << o.x << "/" << o.y << "/" << o.z << std::endl;

		_transforms[i]->setPos(pos);
		_transforms[i]->rotateTo(o);
	}
}
}