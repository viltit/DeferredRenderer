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

	std::vector<vitiGL::SceneNode*>	_nodes;
	vitiGL::SceneNode* _fork;
	
	vitiGEO::SliderConstraint*		_c1;
	vitiGEO::SliderConstraint*		_c2;

	vitiGL::Scene&					_scene;

	bool _motorOn;

	const float _speed = 2.f;
	const float _rotSpeed = 0.5f;

	const glm::vec3 _positions[6] = {
		glm::vec3{ 0.0f, 3.0f, 0.0f },
		glm::vec3{ 0.0f, 3.0f, -1.f },
		glm::vec3{ -1.0f, 3.0f, -1.0f },
		glm::vec3{ -2.0f, 3.0f, -1.0f },
		glm::vec3{ 1.0f, 3.0f, -1.0f },
		glm::vec3{ 2.0f, 3.0f, -1.0f }
	};
};

