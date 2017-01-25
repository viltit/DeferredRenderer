#include "Fork.hpp"

#include <string>
#include <iostream>

using namespace vitiGL;
using namespace vitiGEO;

Fork::Fork(Scene& scene)
	:	 _scene		{ scene },
		_motorOn	{ false }
{}

void Fork::init() {

	std::vector<float> masses;
	/* construct the forks static elements */

	for (size_t i = 0; i < 6; i++) {
		std::string name = "ForkCube" + std::to_string(i);
		_scene.addChild(new Cuboid{ "xml/cube.xml" }, _positions[i], name);
		_nodes.push_back(_scene[name]);
		masses.push_back(100.0f);
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

void Fork::makeChain(const glm::vec3 & startPos, int counter, float distance, const glm::vec3 & scale) {
	static int j{ 0 };
	j++;

	/* create the cubes and add them to the scene: */
	std::vector<PhysicObject*> objs;

	for (size_t i = 0; i < counter; i++) {
		float mass;
		mass = (i == 0) ? 0.0f : 5.0f;

		std::string name = "ChainCube" + std::to_string(j) + "/" + std::to_string(i);
		glm::vec3 pos = { startPos.x, startPos.y - (i * scale.y + distance), startPos.z };

		_scene.addChild(new Model{ "Models/cylinder.obj" }, name, "root");
		_scene[name]->transform.setPos(pos);
		_scene[name]->transform.setScale(scale);
		_scene[name]->addPhysics(BodyType::cylinder, mass);
		objs.push_back(_scene[name]->physics());
	}

	/* now make the chain: */
	CuboidChain* chain = new CuboidChain{ objs, distance };

}


void Fork::onSDLEvent(SDL_Event & input, Camera& cam) {
	_fork->physics()->body()->forceActivationState(1);
	glm::mat3 V = glm::inverse(glm::mat3(cam.getMatrizes().V));
	switch (input.type) {
	case SDL_KEYDOWN:
		switch (input.key.keysym.sym) {
		case SDLK_KP_8:
			_fork->physics()->addVelocity(V * glm::vec3{ 0.0f, 0.0f, -_speed });
			break;
		case SDLK_KP_2:
			_fork->physics()->addVelocity(V * glm::vec3{ 0.0f, 0.0f, _speed });
			break;
		case SDLK_KP_4:
			_fork->physics()->addVelocity(V * glm::vec3{ -_speed, 0.0f, 0.0f });
			break;
		case SDLK_KP_6:
			_fork->physics()->addVelocity(V * glm::vec3{ _speed, 0.0f, 0.0f });
			break;
		case SDLK_KP_DIVIDE:
			_fork->physics()->addVelocity(V * glm::vec3{ 0.0f, _speed, 0.0f });
			break;
		case SDLK_KP_MULTIPLY:
			_fork->physics()->addVelocity(V * glm::vec3{ 0.0f, -_speed, 0.0f });
			break;
		case SDLK_KP_7:
			_fork->physics()->setAngularVelocity(glm::vec3{ 0.0f, 0.0f, -_rotSpeed });
			break;
		case SDLK_KP_9:
			_fork->physics()->setAngularVelocity(glm::vec3{ 0.0f, 0.0f, _rotSpeed });
			break;
		case SDLK_KP_1:
			_fork->physics()->setAngularVelocity(glm::vec3{ -_rotSpeed, 0.0f, 0.0f });
			break;
		case SDLK_KP_3:
			_fork->physics()->setAngularVelocity(glm::vec3{ _rotSpeed, 0.0f, 0.0f });
			break;
		case SDLK_KP_5:
			_fork->physics()->setVelocity(glm::vec3{ 0.0f, 0.0f, 0.0f });
			_fork->physics()->setAngularVelocity(glm::vec3{ 0.0f, 0.0f, 0.0f });
			break;

		case SDLK_KP_0:
			if (_motorOn) {
				_c1->addMotor(10.0f, 1000.0f);
				_c2->addMotor(-10.0f, 1000.0f);

				_motorOn = false;
				std::cout << "Motor off\n";
			}
			else {
				_c1->motorOn();
				_c1->addMotor(-10.0f, 1000.0f);
				_c2->motorOn();
				_c2->addMotor(10.0f, 1000.0f);
				_motorOn = true;
				std::cout << "Motor on\n";
			}
			break;
		}
		break;
	}
}