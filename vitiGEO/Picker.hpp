#pragma once

#include <glm/glm.hpp>
#include <bt/btBulletDynamicsCommon.h>

#include <SDL2\SDL.h>

namespace vitiGEO {

class Picker {
public:
	Picker(btDynamicsWorld* world, const glm::vec3& rayStart, const glm::vec3& rayDir);
	~Picker();

	bool onMouseMove(const glm::vec3& camPos, const glm::vec3& camDir, const SDL_Event& input);

	void remove();

	bool hasBody() { return (_picked) ? true : false; }

private:
	bool move(const btVector3& rayStart, const btVector3& rayDir);

	float						_pickDist;

	btDynamicsWorld*			_world;
	btGeneric6DofConstraint*	_p2p;

	/* store the picked body and its state for proper release: */
	int							_state;
	btVector3					_gravity;
	btRigidBody*				_picked;
};
}
