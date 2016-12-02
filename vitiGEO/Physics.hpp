/*	Class Physics

Task:
-	acts as intermediary between the apps rendering system and the apps physics system
-	right now, the physics system is the BulletPhysics Library:
	http://bulletphysics.org/wordpress/

The idea is to be able to switch the physics library by ONLY adapting this class and
class PhysicObject

*/


#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <bt/btBulletDynamicsCommon.h>


namespace vitiGEO {

	class PhysicObject;
	class Transform;
	class Picker;

	class Physics {
	public:
		static Physics* instance();

		virtual ~Physics();

		void update(unsigned int deltaTime);

		void addObject(PhysicObject* obj);
		void removeObject(PhysicObject* obj);

		/* setters: */
		void setGravity(const glm::vec3& g);

		void picker(const glm::vec3& rayStart, const glm::vec3& rayEnd);

	protected:
		Physics();

		btDefaultCollisionConfiguration*	_config;
		btCollisionDispatcher*				_dispatcher;
		btBroadphaseInterface*				_broadphase;
		btSequentialImpulseConstraintSolver* _solver;
		btDiscreteDynamicsWorld*			_world;

		std::vector<PhysicObject*>			_bodies;

		Picker*								_picker;
	};
}