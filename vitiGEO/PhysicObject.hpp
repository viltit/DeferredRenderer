/*	Class Physic Shape

	Task:	acts as intermediary between the Rendering and the Physics Engine
			look at class Physics for more details							
			-> follows the maker pattern

	Hints:	set mass to 0 to make a static object;

	Gets called from SceneNode::AddPhysics and should not be used directly from
	the main code
*/

#pragma once

#include <glm/glm.hpp>
#include <bt/btBulletDynamicsCommon.h>

#include <vector>
#include <algorithm>

#include "Constraints.hpp"
#include "Transform.hpp"

namespace vitiGEO {

enum class BodyType {
	plane,	/* static object */
	cuboid,
	cone,
	cylinder,
	convexHull,
	sphere,
	mulitBody	/* consists of several other bodies */
};

/*	ABSTRACT BASE CLASS -------------------------------------------------------------------- */

class PhysicObject {
public:
	PhysicObject(Transform* transform, BodyType type);
	virtual ~PhysicObject();

	virtual void remove();
	virtual void update();

	/* wrapper functions accessing btRigidBody Methods: */
	void		setVelocity(const glm::vec3& v)			{ _body->setLinearVelocity(glmVecToBtVec(v)); }
	void		addVelocity(const glm::vec3& dv)		{ _body->setLinearVelocity(_body->getLinearVelocity() + glmVecToBtVec(dv)); }
	glm::vec3	velocity() const						{ return btVecToGlmVec(_body->getLinearVelocity()); }
	void		setAngularVelocity(const glm::vec3& av) { _body->setAngularVelocity(glmVecToBtVec(av)); }
	glm::vec3	angularVelocity() const					{ return btVecToGlmVec(_body-> getAngularVelocity()); }

	void		addImpulse(const glm::vec3& I)			{ _body->applyCentralImpulse(glmVecToBtVec(I)); }
	void		addTorque(const glm::vec3& I)			{ _body->applyTorqueImpulse(glmVecToBtVec(I)); }

	void		setFriction(float f)					{ _body->setFriction(f); }
	float		friction() const						{ return _body->getFriction(); }
	void		setRollingFriction(float f)				{ _body->setRollingFriction(f); }
	float		rollingFriction()						{ return _body->getRollingFriction(); }
	void		setSpinningFriction(float f)			{ _body->setSpinningFriction(f); }
	float		getSpinningFriction()					{ return _body->getSpinningFriction(); }

	void		setDamping(float d)						{ _body->setDamping(d, _body->getAngularDamping()); }
	void		setAngularDamping(float d)				{ _body->setDamping(_body->getLinearDamping(), d); }
	float		damping() const							{ return _body->getLinearDamping(); }
	float		angularDamping() const					{ return _body->getAngularDamping(); }

	void		setGravity(glm::vec3 g)					{ _body->setGravity(glmVecToBtVec(g)); }

	/* access to btRigidBody, btCollisionShape and the Transform Class: */
	btRigidBody* body() const							{ return _body; }
	btCollisionShape* shape() const						{ return _shape; }
	Transform* transform() const						{ return _transform; }

	/* keep track of the Constraints attached to this body: */
	int			numConstraints()						{ return _constraints.size(); }
	Constraint* constraint(int index) {
		if (index >=_constraints.size() || index < 0) return nullptr;
		else return _constraints[index];
	}

	/* add and remove Constraints (used by the Constraint class): */
	void addConstraint(Constraint* c) const					{ _constraints.push_back(c); }
	void removeConstraint(Constraint* c) const { 
		_constraints.erase(std::remove(_constraints.begin(), _constraints.end(), c), _constraints.end()); 
	}

	/* get the type: */
	BodyType type() const { return _type; }

protected:
	Transform*		_transform;
	btRigidBody*	_body;
	btCollisionShape* _shape;

	BodyType 		_type;	

	mutable std::vector<Constraint*> _constraints;
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

private:
};

/*	Cylinder -------------------------------------------------------------------- */
class CylinderObject : public PhysicObject {
public:
	CylinderObject(Transform* transform,
		const void* node,
		float mass,
		const glm::vec3& dimensions,
		const glm::vec3& initVelocity = { 0.0f, 0.0f, 0.0f });

	~CylinderObject();

private:
};

/*	Cone -------------------------------------------------------------------- */
class ConeObject : public PhysicObject {
public:
	ConeObject(Transform* transform,
		const void* node,
		float mass,
		const glm::vec3& dimensions,
		const glm::vec3& initVelocity = { 0.0f, 0.0f, 0.0f });

	~ConeObject();

private:
};

/*	STATIC, INFINITE PLANE ----------------------------------------------------------------- */
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


/*	CONVEX HULL ------------------------------------------------------------------------- */
class ConvexHullObject : public PhysicObject {
public:
	ConvexHullObject(Transform* transform,
					const std::vector<glm::vec3>& points,
					const void* node,
					float mass,
					const glm::vec3& initialVelocity = { 0.0f, 0.0f, 0.0f });

	~ConvexHullObject();

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

private:
};


/*	MULIT-BODY --------------------------------------------------------------------- */
class MultiBody : public PhysicObject {
public:
	MultiBody(Transform* parent, std::vector<Transform*> children,
			  std::vector<glm::vec3> localDimensions,
			  const void* node,
			  std::vector<float> masses,
			  glm::vec3 initialVelocity = {});
	~MultiBody();

	virtual void update() override;

private:
	std::vector<Transform*> _transforms;
	std::vector<float> _masses;
};

}
