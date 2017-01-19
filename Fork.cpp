#include "Fork.hpp"

#include <string>
#include <iostream>

using namespace vitiGL;
using namespace vitiGEO;

Fork::Fork(Scene& scene)
	: _scene{ scene }
{}

void Fork::init() {

	/* construct the forks static elements */
	std::vector<float> masses;
	for (size_t i = 0; i <= 6; i++) {
		std::string name = "ForkCube" + std::to_string(i);
		_scene.addChild(new Cuboid{ "xml/cube.xml" }, _positions[i], name);
		_nodes.push_back(_scene[name]);
		masses.push_back(10.0f);
	}

	/* make a btCompoundShape out of this elements: */
	_scene.addChild(nullptr, glm::vec3{ -0.0f, 3.0f, -0.0f }, "Fork");
	_fork = _scene["Fork"];

	_scene["Fork"]->addCompoundPhysics(_nodes, masses, glm::vec3{ -10.0f, 10.0f, -10.0f });
	_scene["Fork"]->physics()->setGravity(glm::vec3{ 0.0f, 0.0f, 0.0f });

	/* add two cubes with slider constraints on top of the compound: DOES NOT WORK AS I WANT ...*/
	_scene.addChild(new Cuboid{ "xml/cube.xml" }, glm::vec3{ -2.f, 3.0f, -3.0f }, "Slider1");
	_scene["Slider1"]->addPhysics(BodyType::cuboid, 10.0f);
	_scene["Slider1"]->physics()->setGravity(glm::vec3{ 0.0f, 0.0f, 0.0f });

	glm::vec3 inA{ 0.0f, 0.0f, -1.8f };
	glm::vec3 inB{ -0.5f, 0.0f, -0.5f };

	_c1 = new SliderConstraint{
		_scene["Fork"]->physics(), inA, _scene["Slider1"]->physics(), inB, -2.0f, -0.0f
	};

	_scene.addChild(new Cuboid{ "xml/cube.xml" }, glm::vec3{ 2.f, 3.0f, -3.0f }, "Slider2");
	_scene["Slider2"]->addPhysics(BodyType::cuboid, 10.0f);
	_scene["Slider2"]->physics()->setGravity(glm::vec3{ 0.0f, 0.0f, 0.0f });

	inA = glm::vec3{ 0.0f, 0.0f, -1.8f };
	inB = glm::vec3{ 0.5f, 0.0f, -0.5f };

	_c2 = new SliderConstraint{
		_scene["Fork"]->physics(), inA, _scene["Slider2"]->physics(), inB, 0.0f, 2.0f
	};
}

Fork::~Fork() {
	for (size_t i = 0; i < 0; i++) {
		std::string name = "ForkCube" + std::to_string(i);
		_scene.remove(name);
	}
	_scene.remove("Fork");
	
	_nodes.clear();
	_fork = nullptr;

	if (_c1) {
		delete _c2;
		_c2 = nullptr;
	}
	if (_c2) {
		delete _c2;
		_c2 = nullptr;
	}
}


void Fork::onSDLEvent(SDL_Event & input) {
}