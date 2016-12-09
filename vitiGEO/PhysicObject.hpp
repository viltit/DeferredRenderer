/*	Class Physic Shape

	Task:	acts as intermediary between the Rendering and the Physics Engine
			look at class Physics for more details							
			-> follows the maker pattern

	Gets called from SceneNode::AddPhysics and should not be used directly from
	the main code
*/

#pragma once

#include <glm/glm.hpp>
#include <bt/btBulletDynamicsCommon.h>

#include <vector>

#include "Transform.hpp"

namespace vitiGEO {

enum class BodyType {
	plane,
	cuboid,
	convexHull,
	sphere
};

/*	ABSTRACT BASE CLASS -------------------------------------------------------------------- */

class PhysicObject {
public:
	PhysicObject(Transform* transform);
	virtual ~PhysicObject();

	virtual void remove();
	virtual void update() = 0;

	/* wrapper functions accessing btRigidBody Methods: */
	void		setVelocity(const glm::vec3& v)			{ _body->setLinearVelocity(glmVecToBtVec(v)); }
	glm::vec3	velocity() const						{ return btVecToGlmVec(_body->getLinearVelocity()); }
	void		setAngularVelocity(const glm::vec3& av) { _body->setAngularVelocity(glmVecToBtVec(av)); }
	glm::vec3	angularVelocity() const					{ return btVecToGlmVec(_body-> getAngularVelocity()); }

	void		addImpulse(const glm::vec3& I)			{ _body->applyCentralImpulse(glmVecToBtVec(I)); }
	void		addTorque(const glm::vec3& I)			{ _body->applyTorqueImpulse(glmVecToBtVec(I)); }

	void		setFriction(float f)					{ _body->setFriction(f); }
	float		friction() const						{ return _body->getFriction(); }
	void		setRollingFriction(float f)				{ _body->setRollingFriction(f); }
	float		rollingFriction()						{ return _body->getRollingFriction(); }

	void		setDamping(float d)						{ _body->setDamping(d, _body->getAngularDamping()); }
	void		setAngularDamping(float d)				{ _body->setDamping(_body->getLinearDamping(), d); }
	float		damping() const							{ return _body->getLinearDamping(); }
	float		angularDamping() const					{ return _body->getAngularDamping(); }

	btRigidBody* body() { return _body; }

protected:
	Transform*		_transform;
	btRigidBody*	_body;
};


/*	CUBOID -------------------------------------------------------------------- */
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

/*	STATIC PLANE -------------------------------------------------------------------- */
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

/*	CONVEX HULL -------------------------------------------------------------------- */
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

/*	SPHERE ------------------------------------------------------------------------- */
class SphereObject : public PhysicObject {
public:
	SphereObject(Transform* transform,
				const void* node,
				float mass,
				float radius,
				const glm::vec3& initialVelocity = { 0.0f, 0.0f, 0.0f });

	~SphereObject();

	virtual void update() override;

	private:
};

}