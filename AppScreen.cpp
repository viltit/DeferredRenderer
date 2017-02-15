#include "AppScreen.hpp"
#include "vitiGL.hpp"

#include "App.hpp"

#include <conio.h>

#include <glm/glm.hpp>

#include "vitiGEO/Ray.hpp"
#include "vitiGEO/AABB.hpp"
#include "vitiGL/AABBShape.hpp"
#include "vitiGEO/DebugInfo.hpp"
#include "vitiGL/RayTriangle.hpp"
#include "vitiGEO/Physics.hpp"
#include "vitiGEO/Constraints.hpp"

#include <bt/btBulletCollisionCommon.h>

using namespace vitiGL;
using namespace vitiGEO;


AppScreen::AppScreen(App* app, vitiGL::Window* window)
	: IAppScreen{ app },
	_window{ window },
	_cam{ float(window->width()) / float(window->height()) },
	_renderer{ window, &_scene, &_cam },
	_drender{ window, &_scene, &_cam },
	_fork{ _scene }
{
	Physics::instance()->setDebugRenderer(glRendererBTDebug::instance());
	_fork.init();


	_index = SCREEN_INDEX_APP;
	RayTriangle::start();

	/* create some scene elements: 
	bool reverse = false;
	float y0 = 1.0f + 0.25f + 0.03f;
	for (int y = 0; y < 24; y++) {
		if (!reverse) {
			for (int x = 0; x < 4; x++) {
				for (int z = 0; z < 8; z++) {
					std::string name = "Cuboid" + std::to_string(x) + std::to_string(y) + std::to_string(z);
					_scene.addChild(new Cuboid{ "xml/block.xml" }, glm::vec3{ -15.f + 2.f * x, y0 + 0.5f * y, -1.f + 1. * z }, name);
					_scene[name]->addPhysics(BodyType::cuboid, 10.0f);
					//_scene[name]->physics()->body()->setGravity(btVector3(0.0f, 0.0f, 0.f));
				}
			}
		}
		else {
			
			for (int x = 0; x < 8; x++) {
				for (int z = 0; z < 4; z++) {
					std::string name = "Cuboid" + std::to_string(x) + std::to_string(y) + std::to_string(z);
					_scene.addChild(new Cuboid{ "xml/block.xml" }, glm::vec3{ -15.5f + 1.f * x, y0 + 0.5f * y, -.5f + 2. * z }, name);
					_scene[name]->transform.rotate(90.0f, glm::vec3{ 0.0f, 1.0f, 0.0f });
					_scene[name]->addPhysics(BodyType::cuboid, 10.0f);
					//_scene[name]->physics()->body()->setGravity(btVector3(0.0f, 0.0f, 0.f));
				}
			}
		}
		reverse = reverse == true ? false : true;
	}*/

	/* constraint tests: */
	//_scene["CCube1"]->transform.setScale(glm::vec3{ 2.0f, 5.0f, 0.1f });
	//_scene["CCube1"]->addPhysics(BodyType::cuboid, 10.0f);
	//_scene["CCube1"]->physics()->body()->setGravity(btVector3{ 0.0f, 0.0f, 0.0f });
	//vitiGEO::HingeConstraint* c = new vitiGEO::HingeConstraint {
	//	_scene["CCube1"]->physics(), glm::vec3{ 1.1f, 0.0f, 0.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f } };
	//c->addMotor(3.0f, 20.0f);
	//c->setMinMax(-3.1416 / 4.0f, 3.1416 / 4.0f);

	/* compound object test: */


	/* test: load cylinder obj model: */
	_scene.addChild(new Model("Models/cylinder.obj"), "Cylinder");
	_scene["Cylinder"]->transform.setScale(glm::vec3{ 1.0f, 1.5f, 1.0f });
	_scene["Cylinder"]->transform.setPos(glm::vec3{ -5.f, 3.0f, -5.0f });
	_scene["Cylinder"]->addPhysics(BodyType::cylinder, 10.0f);

	_scene.addChild(new Model("Models/cone.obj"), "Cone");
	_scene["Cone"]->transform.setPos(glm::vec3{ -9.f, 3.0f, -5.0f });
	_scene["Cone"]->addPhysics(BodyType::cone, 10.0f);

	_scene.addChild(new Model("Models/torus.obj"), "Torus");
	_scene["Torus"]->transform.setPos(glm::vec3{ -12.f, 3.0f, -7.0f });
	_scene["Torus"]->addPhysics(BodyType::convexHull, 10.0f);

	_scene.addChild(new Cuboid{ "xml/cube_floor.xml" }, glm::vec3{ 0.0f, 1.0f, 0.0f }, "Floor");
	_scene["Floor"]->addPhysics(BodyType::cuboid, 0.0f);

	_scene.addChild(new Cuboid{ "xml/cube_floorBig.xml" }, glm::vec3{ 0.0f, -10.0f, 0.0f }, "Floor2");
	_scene["Floor2"]->addPhysics(BodyType::cuboid, 0.0f);

	_scene.addChild(new Cuboid{ "xml/cube_floor.xml" }, glm::vec3{ 7.0f, 7.0f, 0.0f }, "Wall");
	_scene["Wall"]->transform.rotateTo(45.0f, glm::vec3{ 0.0f, 0.0f, 1.0f });
	_scene["Wall"]->addPhysics(BodyType::cuboid, 0.0f);

	/* add a directional and a point light: */
	dLight* dlight = new dLight{ "dlight", glm::vec3{ 0.5f, -1.0f, -0.5f } };
	dlight->setProperty(lightProps::diffuse, glm::vec3{ 0.8f, 0.1f, 0.0f });
	dlight->setProperty(lightProps::specular, glm::vec3{ 1.f, 0.0f, 0.0f });
	_scene.addChild(dlight, "dlight");
	_scene.setShadowcaster("dlight");

	pLight* plight = new pLight{ &_cam };
	plight->setProperty(lightProps::pos, glm::vec3{ 0.0f, 15.0f, 10.0f });
	plight->setProperty(lightProps::diffuse, glm::vec3{ 10.0f, 5.0f, 0.0f });
	plight->setProperty(lightProps::specular, glm::vec3{ 20.0f, 10.0f, 0.0f });
	plight->setProperty(lightProps::attenuation, Attenuation::r160);

	_scene.addChild(plight, "plight");
	_scene.setShadowcaster("plight");

	/* add Skybox: */
	std::vector<std::string> faces;
	faces.push_back("Textures/Skybox/right.jpg");
	faces.push_back("Textures/Skybox/left.jpg");
	faces.push_back("Textures/Skybox/top.jpg");
	faces.push_back("Textures/Skybox/bottom.jpg");
	faces.push_back("Textures/Skybox/back.jpg");
	faces.push_back("Textures/Skybox/front.jpg");
	Skybox* skybox = new Skybox(faces);
	_scene.addChild(skybox, "Skybox");

	_scene.addCamera(&_cam);

	_cam.setPos(glm::vec3{ -4.0f, 8.0f, -5.0f });
	//_cam.setTarget(_scene["Cuboid000"]->transform.pos());

	_timer.on();

	/* debug: 
	_scene.print();*/
}


AppScreen::~AppScreen() {
}

void AppScreen::onEntry() {
	SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_CaptureMouse(SDL_TRUE);
	Physics::instance()->startSimulation();
	if (_timer.is_paused()) _timer.unpause();
}

void AppScreen::onExit() {
	SDL_CaptureMouse(SDL_FALSE);
	Physics::instance()->stopSimulation();
	_timer.pause();
}

void AppScreen::update() {
	Uint32 frameTime = _timer.frame_time();
	Uint32 time = _timer.get_time();

	std::string fps = "FPS: " + std::to_string(_timer.fps());

	/* update all components: */
	Physics::instance()->update(frameTime);
	_cam.update();

	updateInput();
	_scene.update(frameTime);
}

void AppScreen::updateFreezed() {
	Uint32 frameTime = _timer.frame_time();
	Physics::instance()->update(frameTime);
	updateInput();
	_scene.update(frameTime);
}

void AppScreen::draw() {
	_drender.draw();
}

int AppScreen::next() const {
	return SCREEN_INDEX_NONE;
}

int AppScreen::previous() const {
	return SCREEN_INDEX_MENU;
}

void AppScreen::updateInput() {
	SDL_Event input;
	while (SDL_PollEvent(&input)) {
		_fork.onSDLEvent(input, _cam);

		//temporary solution for object picking:
		Physics::instance()->update(_cam.pos(), _cam.dir(), input);

		switch (input.type) {
		case SDL_QUIT:
			_state = ScreenState::exit;
			break;
		case SDL_KEYDOWN:
			switch (input.key.keysym.sym) {
			case SDLK_ESCAPE:
				_state = ScreenState::previous;
				break;
			case SDLK_w:
				_cam.move(Move::forward);
				break;
			case SDLK_s:
				_cam.move(Move::backward);
				break;
			case SDLK_a:
				_cam.move(Move::left);
				break;
			case SDLK_d:
				_cam.move(Move::right);
				break;
			case SDLK_r:
				break;
			case SDLK_t:
				//_scene["Wall"]->transform.rotateTo(0.0f, glm::vec3{ 0.0f, 0.0f, 1.0f });
				break;
			case SDLK_KP_7:
				break;
			case SDLK_KP_1:
				//_btBodies["Cuboid"]->applyCentralImpulse(btVector3{ 0.0f, -100.0f, 0.0f });
				break;
			case SDLK_KP_9:
			{
			}
			case SDLK_KP_3:
				//_btBodies["Cuboid"]->applyTorque(btVector3{ 0.0f, -10.0f, 0.0f });
				break;
			case SDLK_KP_5:
				//_scene["Cuboid"]->transform.setWorldMatrix(glm::mat4{});
				break;
			case SDLK_SPACE:
				addCube(10.0f, _cam.pos());
				break;
			case SDLK_LSHIFT:
				addIcosahedron(4.0f, _cam.pos());
				break;
			case SDLK_LALT:
				addTetrahedron(1.0, _cam.pos());
				break;
			case SDLK_F1:
				break;
			case SDLK_F2:
				break;
			case SDLK_g:
				break;
			case SDLK_F3:
				break;
			case SDLK_F4:
				break;
			case SDLK_F5:
				break;
			case SDLK_F6:
				break;
			case SDLK_F7:
				break;
			case SDLK_F9:
				break;
			case SDLK_KP_PLUS:
				break;
			case SDLK_KP_MINUS:
				break;
			case SDLK_KP_MULTIPLY:
				break;
			case SDLK_KP_DIVIDE:
				break;
			}
			break;
		case SDL_MOUSEMOTION:
			int dx, dy;
			SDL_GetMouseState(&dx, &dy);
			std::cout << "Mouse pos: " << dx << "/" << dy << std::endl;
			_cam.rotate(dx, dy);
			/* reset the mouse to the screen-center:*/
			_window->centerMouse();
			break;
		case SDL_MOUSEWHEEL:
			_cam.zoom(input.wheel.y);
			break;
		case SDL_MOUSEBUTTONDOWN:
			switch (input.button.button) {
			case SDL_BUTTON_LEFT:
			{
				/* get the world space position of the mouse: */
				glm::vec3 rayS = _cam.pos();
				glm::vec3 rayDir = _cam.dir();
				Physics::instance()->picker(rayS, rayDir);
			}
				break;
			case SDL_BUTTON_RIGHT:
				break;
			}
		}
	}
}

void AppScreen::addCube(float mass, const glm::vec3& pos) {
	static int i = 0;
	std::string name = "Cube" + std::to_string(i++);

	glm::vec3 v = _cam.dir() * 10.0f;

	_scene.addChild(new Model{ "models/unitCube/unitCube.obj" }, name);
	_scene[name]->transform.setPos(pos);
	_scene[name]->addPhysics(BodyType::cuboid, 10.0f, v);

	/*
	static int i = 0;
	std::string name = "Cube" + std::to_string(i++);

	glm::vec3 v = _cam.dir() * 10.0f;

	_scene.addChild(new Cuboid{ "xml/cube.xml" }, pos, name);
	_scene[name]->transform.setScale(glm::vec3{ 1.0f, 2.0f, 0.5f });
	_scene[name]->addPhysics(BodyType::cuboid, 10.0f, v);*/
}

void AppScreen::addOctahedron(float mass, const glm::vec3 & pos) {
	static int i = 0;
	std::string name = "Octahedron" + std::to_string(i++);
	glm::vec3 v = _cam.dir() * 10.0f;

	_scene.addChild(new Octahedron{ "xml/cubeSmall.xml" }, pos, name);
	_scene[name]->addPhysics(BodyType::convexHull, mass, v);
}

void AppScreen::addTetrahedron(float mass, const glm::vec3& pos) {
	static int i = 0;
	std::string name = "Tetrahedron" + std::to_string(i++);
	glm::vec3 v = _cam.dir() * 10.0f;

	_scene.addChild(new Tetrahedron{ "xml/cubeTiny.xml" }, pos, name);
	_scene[name]->addPhysics(BodyType::convexHull, mass, v);
}

void AppScreen::addIcosahedron(float mass, const glm::vec3 & pos) {
	static int i = 0;
	std::string name = "Icosahedron" + std::to_string(i++);
	glm::vec3 v = _cam.dir() * 10.0f;

	_scene.addChild(new Icosahedron{ "xml/cubeTiny.xml" }, pos, name);
	_scene[name]->transform.setScale(glm::vec3{ 2.0f, 2.0f, 2.0f }
	);
	_scene[name]->addPhysics(BodyType::sphere, mass, v);
	_scene[name]->physics()->setRollingFriction(0.3f);
}

void AppScreen::addChain(const glm::vec3 & startPos, int counter, float distance, const glm::vec3& scale) {
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