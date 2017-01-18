#pragma once

#include <vector>

#include <bt/btBulletCollisionCommon.h>
#include <vitiGL.hpp>
#include <PhysicObject.hpp>

class Fork {
public:
	Fork(vitiGL::Scene* scene);
	~Fork();

private:
	std::vector<vitiGL::Cuboid*>	_cubes;
	
	vitiGEO::SliderConstraint*		_c1;
	vitiGEO::SliderConstraint*		_c2;

	vitiGL::Scene*					_scene;

	const glm::vec3 _positions[6] = {
		glm::vec3{ 0.0f, 3.0f, -1.f },
		glm::vec3{ -1.0f, 3.0f, -1.0f },
		glm::vec3{ -2.0f, 3.0f, -1.0f },
		glm::vec3{ 1.0f, 3.0f, -1.0f },
		glm::vec3{ 2.0f, 3.0f, -1.0f }
	};
};

