#include "PhysicObject.hpp"

#include "Physics.hpp"
#include "DebugInfo.hpp"

#include <algorithm>

namespace vitiGEO {

std::string bodyTypeToString(BodyType type) {
	switch (type) {
		case BodyType::plane: return "Plane";
		case BodyType::cuboid: return "Cuboid";
		case BodyType::cone: return "Cone";
		case BodyType::cylinder: return "Cylinder";
		case BodyType::convexHull: return "ConvexHull";
		case BodyType::sphere: return "Sphere";
		case BodyType::mulitBody: return "MultiBody";
		//default:
			//throw vitiError("<bodyTypeToString> Unknown body type.");			  
	}
}

BodyType stringToBodyType(const std::string& name) {
	if (name == "plane") return BodyType::plane;
	else if (name == "cuboid") return BodyType::cuboid;
	else if (name == "cone") return BodyType::cone;
	else if (name == "cylinder") return BodyType::cylinder;
	else if (name == "convexHull") return BodyType::convexHull;
	else if (name == "sphere") return BodyType::sphere;
	else if (name == "multiBody") return BodyType::mulitBody;	
}

	PhysicObject::PhysicObject(Transform* transform, BodyType type)
		: _transform{ transform },
		  _body{ nullptr },
		  _shape{ nullptr },
		  _type{ type }
	{}

	PhysicObject::~PhysicObject() {
	}

	void PhysicObject::remove() {
		Physics::instance()->removeObject(this);
	}

	void PhysicObject::update() {
		btTransform t;
		_body->getMotionState()->getWorldTransform(t);

		/* adapt position: */
		glm::vec3 pos = btVecToGlmVec(_body->getCenterOfMassPosition());
		_transform->setPos(pos);

		/* adapt orientation: */
		glm::quat o2 = btQuatToGlmQuat(t.getRotation());
		_transform->rotateTo(o2);
	}

	/* Cuboid ------------------------------------------------------------------------------------ */
	CuboidObject::CuboidObject(Transform * transform,
		const void* node,
		float mass,
		const glm::vec3& dimensions,
		const glm::vec3 & initVelocity)

		: PhysicObject{ transform, BodyType::cuboid }
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


	/* Plane ------------------------------------------------------------------------------------ */
	PlaneObject::PlaneObject(Transform * transform,
		const void* node,
		float d,
		const glm::vec3 & normal)

		: PhysicObject{ transform, BodyType::plane }
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


	/*---------------------CONVEX HULL ------------------------------------  */
	ConvexHullObject::ConvexHullObject(
		Transform * transform,
		const std::vector<glm::vec3>& points,
		const void * node, float mass,
		const glm::vec3 & initialVelocity)

		: PhysicObject{ transform, BodyType::convexHull }
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

/*---------------------------------SPHERE ---------------------------------------------  */
	SphereObject::SphereObject(Transform * transform, const void * node, float mass, float radius, const glm::vec3& initialVelocity)
		: PhysicObject{ transform, BodyType::sphere }
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

	/*	MULIT-BODY --------------------------------------------------------------------- */

	MultiBody::MultiBody(Transform* parent,
		std::vector<Transform*> children,
		std::vector<glm::vec3> localDimensions,
		const void* node,
		std::vector<float> mass,
		glm::vec3 initialVelocity)

		: PhysicObject{ parent, BodyType::mulitBody },
		_transforms{ children },
		_masses{ mass }
	{
		//assert(transforms.size() == mass.size() == localDimensions.size());

		btCompoundShape* shape = new btCompoundShape{};
		float massSum{ 0.0f };

		for (size_t i = 0; i < children.size(); i++) {
			btBoxShape* child = new btBoxShape{ glmVecToBtVec(children[i]->scale() * localDimensions[i] / 2.0f) };
			child->setMargin(0.01f);

			btTransform t;
			t.setIdentity();
			t.setOrigin(glmVecToBtVec(children[i]->pos()));
			t.setRotation(glmQuatToBtQuat(children[i]->orientation()));

			shape->addChildShape(t, child);

			massSum += mass[i];
		}

		btVector3 inertia;
		btTransform principal;

		shape->calculatePrincipalAxisTransform(mass.data(), principal, inertia);

		/* we need to update all child transforms with the inverse of the principal:*/
		for (size_t i = 0; i < children.size(); i++) {
			btTransform newChildTransform = principal.inverse()*shape->getChildTransform(i);
			shape->updateChildTransform(i, newChildTransform);
		}

		btTransform t;
		t.setIdentity();
		t.setOrigin(glmVecToBtVec(parent->pos()));
		t.setRotation(glmQuatToBtQuat(parent->orientation()));

		btMotionState* motion = new btDefaultMotionState{ t };


		_shape = shape;

		_body = new btRigidBody{ massSum, motion, _shape, inertia };

		_body->setLinearFactor(btVector3{ 0.8f, 0.8f, 0.8f });
		_body->setCollisionFlags(_body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
		_body->setUserPointer((void*)node);
		_body->setLinearVelocity(glmVecToBtVec(initialVelocity));


		Physics::instance()->addObject(this);
	}

	MultiBody::~MultiBody() {
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


	void MultiBody::update() {
		size_t numChildren = static_cast<btCompoundShape*>(_shape)->getNumChildShapes();
		btCompoundShape* shape = static_cast<btCompoundShape*>(_shape);

		/* get the parents transform: */
		btTransform parent;
		_body->getMotionState()->getWorldTransform(parent);

		glm::vec3 parentPos = btVecToGlmVec(parent.getOrigin());
		glm::quat parentRot = btQuatToGlmQuat(parent.getRotation());

		_transform->rotateTo(parentRot);
		_transform->setPos(parentPos);

		/* get the local transforms of all children: */
		for (size_t i = 0; i < numChildren; i++) {
			btTransform child = static_cast<btCompoundShape*>(_shape)->getChildTransform(i);

			glm::vec3 pos = btVecToGlmVec(child.getOrigin());
			glm::quat rot = btQuatToGlmQuat(child.getRotation());

			_transforms[i]->setPos(pos);
			_transforms[i]->rotateTo(rot);
		}
	}

	/*	CYLINDER --------------------------------------------------------------------- */

	CylinderObject::CylinderObject(
		Transform * transform, 
		const void * node, 
		float mass, 
		const glm::vec3 & dimensions, 
		const glm::vec3 & initVelocity)

		: PhysicObject(transform, BodyType::cylinder)

	{
		/* adapt orientation and position: */
		btTransform t;

		t.setIdentity();
		t.setOrigin(glmVecToBtVec(transform->pos()));
		t.setRotation(glmQuatToBtQuat(transform->orientation()));

		_shape = new btCylinderShape(glmVecToBtVec(_transform->scale() * dimensions / 2.0f));

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


	CylinderObject::~CylinderObject() {
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

 	/* -----------------------------CONE ----------------------------*/
	ConeObject::ConeObject(
		Transform * transform, 
		const void * node, 
		float mass, 
		const glm::vec3 & dimensions, 
		const glm::vec3 & initVelocity)

		: PhysicObject(transform, BodyType::cone)
	{
		/* adapt orientation and position: */
		btTransform t;

		t.setIdentity();
		t.setOrigin(glmVecToBtVec(transform->pos()));
		t.setRotation(glmQuatToBtQuat(transform->orientation()));

		_shape = new btConeShape(transform->scale().x * dimensions.x * 0.5f, transform->scale().y * dimensions.y);

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

	ConeObject::~ConeObject() {
	}
}
