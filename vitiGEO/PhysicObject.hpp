/*	Class should act as intermediary between the Rendering and the Physics Engine
	look at class Physics for more details										*/

#pragma once

#include <glm/glm.hpp>
#include <bt/btBulletDynamicsCommon.h>

#include <vector>

#include "Transform.hpp"

namespace vitiGEO {

enum class BodyType {
	plane,
	cuboid,
	convexHull
};

class PhysicObject {
public:
	PhysicObject(Transform* transform);
	virtual ~PhysicObject();

	virtual void remove();
	virtual void update() = 0;

	void setVelocity(const glm::vec3& v)			{ _body->setLinearVelocity(glmVecToBtVec(v)); }
	void setAngularVelocity(const glm::vec3& av)	{ _body->setAngularVelocity(glmVecToBtVec(av)); }
	void addImpulse(const glm::vec3& I) { _body->applyCentralImpulse(glmVecToBtVec(I)); }


	btRigidBody* body() { return _body; }

protected:
	Transform*		_transform;
	btRigidBody*	_body;
};

class CuboidObject : public PhysicObject {
public:
	CuboidObject(Transform* transform,
				const void* node,
				float mass,
				const glm::vec3& dimensions,
				const glm::vec3& initVelocity = { 0.0f, 0.0f, 0.0f });

	~CuboidObject();

	virtual void update() override;

private:
};


class PlaneObject : public PhysicObject {
public:
	PlaneObject(Transform* transform,
		const void* node,
		float planeD,
		const glm::vec3 & planeNormal);

	~PlaneObject();

	/* the plane is static, so no update needed */
	virtual void update() override;

private:
};


class ConvexHullObject : public PhysicObject {
public:
	ConvexHullObject(Transform* transform,
					const std::vector<glm::vec3>& points,
					const void* node,
					float mass,
					const glm::vec3& initialVelocity = { 0.0f, 0.0f, 0.0f });

	~ConvexHullObject();

	virtual void update() override;

private:
};


}