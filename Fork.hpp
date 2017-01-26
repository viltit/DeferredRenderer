#pragma once

#include <vector>

#include <bt/btBulletCollisionCommon.h>
#include <vitiGL.hpp>
#include <PhysicObject.hpp>

class Fork {
public:
	Fork(vitiGL::Scene& scene);

	void init();
	void onSDLEvent(SDL_Event& input, vitiGL::Camera& cam);

	~Fork();

private:
	void makeChain(const glm::vec3 & startPos, int counter, float distance, const glm::vec3& scale);

	vitiGEO::CuboidChain*	_chain;
	vitiGL::SceneNode*		_fork;
	
	vitiGEO::SliderConstraint*		_c1;
	vitiGEO::SliderConstraint*		_c2;

	vitiGL::Scene&					_scene;

	bool _motorOn;

	const float _speed = 2.f;
	const float _rotSpeed = 0.5f;
};

