#include "Fork.hpp"

#include <string>
#include <iostream>

using namespace vitiGL;
using namespace vitiGEO;

Fork::Fork(Scene* scene) 
	: _scene{ scene }
{
	std::cout << "Constructing fork\n";

	std::vector<SceneNode*> nodes;
	std::vector<float> masses;

	/* Construct the forks elements: */
	for (size_t i = 0; i < 6; i++) {
		std::string name = "Fork[" + std::to_string(i) + ']';
		_cubes.push_back(new Cuboid{ "xml/cube.xml" });
		scene->addChild(_cubes[i], _positions[i], name);
		nodes.push_back(scene->findByName(name));
		masses.push_back(10.0f);
	}

	/* make a physics compound objects out of the elements: */
	scene->addChild(nullptr, glm::vec3{ 0.0f, 3.0f, 0.0f }, "Fork");
	scene->findByName("Fork")->addCompoundPhysics(nodes, masses, glm::vec3{ -10.0f, 10.0f, -10.0f });

	/* add two cubes with slider constraints on top: */
	_cubes.push_back(new Cuboid{ "xml/cube.xml" });
	scene->addChild(_cubes[_cubes.size() - 1], glm::vec3{ -2.f, 3.0f, -3.0f }, "ForkSlider[1]");
	scene->findByName("ForkSlider[1]")->addPhysics(BodyType::cuboid, 10.0f);

	glm::vec3 inA{ 0.0f, 0.0f, -1.8f };
	glm::vec3 inB{ -0.5f, 0.0f, -0.5f };

	_c1 = new SliderConstraint {
		scene->findByName("Fork")->physics(), inA, scene->findByName("ForkSlider[1]")->physics(), inB, -2.0f, -0.0f
	};

	_cubes.push_back(new Cuboid{ "xml/cube.xml" });
	scene->addChild(_cubes[_cubes.size() - 1], glm::vec3{ 2.f, 3.0f, -3.0f }, "ForkSlider[2]");
	scene->findByName("ForkSlider[2]")->addPhysics(BodyType::cuboid, 10.0f);

	inA = glm::vec3{ 0.0f, 0.0f, -1.8f };
	inB = glm::vec3{ 0.5f, 0.0f, -0.5f };

	_c1 = new SliderConstraint{
		scene->findByName("Fork")->physics(), inA, scene->findByName("ForkSlider[2]")->physics(), inB, -2.0f, -0.0f
	};

	std::cout << "Constructing fork finished\n";
}

Fork::~Fork() {
	for (size_t i = 0; i < 0; i++) {
		std::string name = "Fork[" + i + ']';
		_scene->remove(name);
	}
	
	if (_c1) {
		delete _c2;
		_c2 = nullptr;
	}
	if (_c2) {
		delete _c2;
		_c2 = nullptr;
	}
}
