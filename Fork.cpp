#include "Fork.hpp"

#include <string>
#include <iostream>

using namespace vitiGL;
using namespace vitiGEO;

Fork::Fork(Scene& scene)
	:	_scene		{ scene },
		_motorOn	{ false },
		_chain		{ nullptr },
		_fork		{ nullptr },
		_c1			{ nullptr },
		_c2			{ nullptr }
{}

void Fork::init() {
	/* make the chain the grappler hangs on: */
	makeChain(glm::vec3{ -3.0f, 15.0f, -3.0f }, 8, 0.0f, glm::vec3{ 0.2f, 0.5f, 0.2f });

	/* add the grapplers arch: */
	_scene.addChild(new Model{ "Models/arch.obj" }, "Arch");
	_scene["Arch"]->transform.setPos(glm::vec3{ -12.f, 4.f, -12.f });
	_scene["Arch"]->addPhysics(BodyType::convexHull, 10.0f);
	_scene["Arch"]->physics()->body()->setDamping(0.1f, 0.1f);

	Constraint* archConstraint1 = new P2PConstraint(
		_chain->last(), glm::vec3{ -0.2f, -0.4f, 0.0f },
		_scene["Arch"]->physics(), glm::vec3{ -0.3f, 1.3f, 0.0f });

	Constraint* archConstraint2 = new P2PConstraint(
		_chain->last(), glm::vec3{ 0.2f, -0.4f, 0.0f },
		_scene["Arch"]->physics(), glm::vec3{ 0.3f, 1.3f, 0.0f });

	/* add the two grappler A 
	_scene.addChild(new Cuboid{ "xml/cube.xml" }, "Grappler_A");
	_scene["Grappler_A"]->transform.setScale(glm::vec3{ 0.5f, 2.f, 0.5f });
	_scene["Grappler_A"]->transform.setPos(glm::vec3{ -12.f, 2.f, -12.f });
	_scene["Grappler_A"]->addPhysics(BodyType::cuboid, 5.0f);

	_c1 = new HingeConstraint{ 
		_scene["Grappler_A"]->physics(), glm::vec3{ 0.0f, 1.01f, 0.0f }, glm::vec3{ 1.0f, 0.0f, 0.0f },
		_scene["Arch"]->physics(), glm::vec3{ 0.0f, -0.65f, 1.15f }, glm::vec3{ 1.0f, 0.0f, 0.0f }
	};
	_c1->setMinMax(0.0f, glm::radians(45.0f));*/

	/* Add grappler B: 
	_scene.addChild(new Cuboid{ "xml/cube.xml" }, "Grappler_B");
	_scene["Grappler_B"]->transform.setScale(glm::vec3{ 0.5f, 2.f, 0.5f });
	_scene["Grappler_B"]->transform.setPos(glm::vec3{ -10.f, 2.f, -10.f });
	_scene["Grappler_B"]->addPhysics(BodyType::cuboid, 5.0f);

	_c2 = new HingeConstraint{
		_scene["Grappler_B"]->physics(), glm::vec3{ 0.0f, 1.01f, 0.0f }, glm::vec3{ 1.0f, 0.0f, 0.0f },
		_scene["Arch"]->physics(), glm::vec3{ 0.0f, -0.65f, -1.15f }, glm::vec3{ 1.0f, 0.0f, 0.0f }
	};
	_c2->setMinMax(glm::radians(-45.0f), 0.0f);*/
}

Fork::~Fork() {
	if (_fork) {
		_scene.remove("Fork");
		_fork = nullptr;
	}

	if (_chain) {
		delete _chain;
		_chain = nullptr;
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

void Fork::makeChain(const glm::vec3 & startPos, int counter, float distance, const glm::vec3 & scale) {
	static int j{ 0 };
	j++;

	/* create the cylinders objects for the chain and add them to the scene: */
	std::vector<PhysicObject*> objs;

	for (size_t i = 0; i < counter; i++) {
		float mass;
		mass = (i == 0) ? 0.0f : 2.0f;

		std::string name = "ChainCube" + std::to_string(j) + "/" + std::to_string(i);
		glm::vec3 pos = { startPos.x, startPos.y - (i * scale.y + distance), startPos.z };

		_scene.addChild(new Model{ "Models/cylinder.obj" }, name, "root");
		_scene[name]->transform.setPos(pos);
		_scene[name]->transform.setScale(scale);
		_scene[name]->addPhysics(BodyType::cylinder, mass);
		_scene[name]->physics()->body()->setDamping(0.1f, 0.1f);
		objs.push_back(_scene[name]->physics());
	}

	/* now make the chain: */
	_chain = new CuboidChain{ objs, distance };
}


void Fork::onSDLEvent(SDL_Event & input, Camera& cam) {
	glm::mat3 V = glm::inverse(glm::mat3(cam.getMatrizes().V));
	switch (input.type) {
	case SDL_KEYDOWN:
		_chain->first()->body()->forceActivationState(1);
		glm::vec3 dv{};
		switch (input.key.keysym.sym) {
		case SDLK_KP_8:
			dv = V * glm::vec3{ 0.0f, 0.0f, -_speed };
			dv.y = 0.0f; //we dont want movment in the z-Axis
			_chain->first()->addVelocity(dv);
			break;
		case SDLK_KP_2:
			dv = V * glm::vec3{ 0.0f, 0.0f, _speed };
			dv.y = 0.0;
			_chain->first()->addVelocity(dv);
			break;
		case SDLK_KP_4:
			dv = V * glm::vec3{ -_speed, 0.0f, 0.0f };
			dv.y = 0.0f;
			_chain->first()->addVelocity(dv);
			break;
		case SDLK_KP_6:
			dv = V * glm::vec3{ _speed, 0.0f, 0.0f };
			dv.y = 0.0f;
			_chain->first()->addVelocity(dv);
			break;
		case SDLK_KP_DIVIDE:
			dv.y = _speed;
			_chain->first()->addVelocity(dv);
			break;
		case SDLK_KP_MULTIPLY:
			dv.y = -_speed;
			_chain->first()->addVelocity(dv);
			break;
		case SDLK_KP_7:
			_chain->first()->setAngularVelocity(glm::vec3{ 0.0f, 0.0f, -_rotSpeed });
			break;
		case SDLK_KP_9:
			_chain->first()->setAngularVelocity(glm::vec3{ 0.0f, 0.0f, _rotSpeed });
			break;
		case SDLK_KP_5:
			break;
		case SDLK_KP_0:
			if (_motorOn) {
				_c1->addMotor(1.0f, 100.0f);
				_c2->addMotor(-1.0f, 100.0f);

				_motorOn = false;
				std::cout << "Motor off\n";
			}
			else {
				_c1->motorOn();
				_c1->addMotor(-1.0f, 100.0f);
				_c2->motorOn();
				_c2->addMotor(1.0f, 100.0f);
				_motorOn = true;
				std::cout << "Motor on\n";
			}
			break;
		}
		break;
	}
}