#include "AppScreen.hpp"
#include "vitiGL.hpp"

#include "App.hpp"

#include <conio.h>

#include "vitiGEO/Ray.hpp"
#include "vitiGEO/AABB.hpp"
#include "vitiGL/AABBShape.hpp"
#include "vitiGEO/DebugInfo.hpp"
#include "vitiGL/RayTriangle.hpp"
#include "vitiGEO/Physics.hpp"

#include <bt/btBulletCollisionCommon.h>

using namespace vitiGL;
using namespace vitiGEO;

bool collisionCallback(btManifoldPoint& pt, const btCollisionObjectWrapper* obj0, int id0, int index0,
	const btCollisionObjectWrapper* obj1, int id1, int index1) {
	std::cout << "Collision\n";
	return false;
}

AppScreen::AppScreen(App* app, vitiGL::Window* window)
	: IAppScreen{ app },
	_window{ window },
	_cam{ float(window->width()) / float(window->height()) },
	_renderer{ window, &_scene, &_cam },
	_drender{ window, &_scene, &_cam },
	_gui{ "GUI", "TaharezLook.scheme" },
	_console{ this, &_gui, "layouts/console.layout" }
{

	gContactAddedCallback = collisionCallback;

	_index = SCREEN_INDEX_APP;
	RayTriangle::start();

	/* create some scene elements: */
	_scene.addChild(new Cuboid{ "xml/cube.xml" }, glm::vec3{ 0.0f, 5.0f, -1.0f }, "Cuboid");
	/* TO DO: physics does not scale yet: */
	_scene["Cuboid"]->transform.setScale(glm::vec3{ 1.0f, 10.0f, 0.5f });
	_scene["Cuboid"]->addPhysics(BodyType::cuboid, 10.0f);

	_scene.addChild(new Cuboid{ "xml/cube_floor.xml" }, glm::vec3{ 0.0f, 1.0f, 0.0f }, "Floor");
	_scene["Floor"]->addPhysics(BodyType::plane, 1.0f, glm::vec3{ 0.0f, 1.0f, 0.0f });

	_scene.addChild(new Cuboid{ "xml/cube_floor.xml" }, glm::vec3{ 7.0f, 7.0f, 0.0f }, "Wall");
	_scene["Wall"]->transform.rotateTo(90.0f, glm::vec3{ 0.0f, 0.0f, 1.0f });
	_scene["Wall"]->addPhysics(BodyType::plane, -7.0f, glm::vec3{ 0.0f, 1.0f, 0.0f });

	/* add a directional and a point light: */
	_scene.addChild(new dLight{ "dlight", glm::vec3{ 0.5f, -1.0f, -0.5f } }, "dlight");
	_scene.setShadowcaster("dlight");

	pLight* plight = new pLight{ &_cam };
	plight->setProperty(lightProps::pos, glm::vec3{ 0.0f, 12.0f, 0.0f });
	plight->setProperty(lightProps::diffuse, glm::vec3{ 10.0f, 5.0f, 0.0f });
	plight->setProperty(lightProps::specular, glm::vec3{ 20.0f, 10.0f, 0.0f });

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
	_cam.setTarget(glm::vec3{ 0.0f, 0.0f, 0.0f });

	initGUI();
	_timer.on();
}


AppScreen::~AppScreen() {
}

void AppScreen::onEntry() {
	SDL_ShowCursor(0);

	if (_timer.is_paused()) _timer.unpause();
}

void AppScreen::onExit() {
	globals::screenshot = _drender.texture();

	_timer.pause();
}

void AppScreen::update() {
	Uint32 frameTime = _timer.frame_time();
	Uint32 time = _timer.get_time();

	std::string fps = "FPS: " + std::to_string(_timer.fps());
	_console.setText(fps);
	_gui.update(frameTime);

	/* update all components: */
	updateInput();
	Physics::instance()->update(frameTime);

	//PhysicEngine::instance()->update(frameTime);

	//DEBUG: Track the cube
	DebugInfo::instance()->addLine(glm::vec4{ _cam.pos() + _cam.dir(), 0.05f }, glm::vec4{ _scene["Cuboid"]->transform.pos(), 0.05f });

	_scene.update(frameTime);
	_cam.update();
	_gui.update(frameTime);
}

void AppScreen::draw() {
	_drender.draw();
	_gui.draw();
}

int AppScreen::next() const {
	return SCREEN_INDEX_NONE;
}

int AppScreen::previous() const {
	return SCREEN_INDEX_MENU;
}

/* very simple command parser for console input: */
std::string AppScreen::command(const std::string & input) {

	/* we need a lot of string manipulation here... */
	std::string::size_type commandEnd = input.find(" ", 1);
	std::string command = input.substr(0, commandEnd);
	std::string argument = input.substr(commandEnd + 1, input.length() - commandEnd + 1);

	float value = atof(argument.c_str());

	std::cout << "Command: " << command << " /Value: " << argument << std::endl;

	/* convert command to lower-case: */
	for (size_t i = 0; i < command.size(); i++) {
		command[i] = tolower(command[i]);
	}

	/* process command: */
	if (command == "exit" || command == "quit") {
		_state = ScreenState::exit;
		return "Exiting app";
	}
	if (command == "setgamma") {
		_drender.setGamma(value);
		return "Setting gamma correction to " + std::to_string(value);
	}
	if (command == "getgamma") {
		return std::to_string(_drender.gamma());
	}
	if (command == "setexposure") {
		_drender.setExposure(value);
		return "Setting hdr exposure to " + std::to_string(value);
	}
	if (command == "getexposure") {
		return std::to_string(_drender.exposure());
	}
	if (command == "setbloom") {
		_drender.setBloomTreshold(value);
		return "Setting bloom treshold to " + std::to_string(value);
	}
	if (command == "getbloom") {
		return std::to_string(_drender.bloomTreshold());
	}
	if (command == "glerror") {
		return std::to_string(glGetError());
	}
	if (command == "setwireframe") {
		_drender.setDrawMode();
		return "Switching between wireframe and normal drawing...";
	}
	if (command == "drawnormals") {
		_drender.drawNormals(true);
		return "Drawing objects vertex normals.";
	}
	if (command == "hidenormals") {
		_drender.drawNormals(false);
		return "Hiding objects vertex normals.";
	}

	return "";
}

void AppScreen::initGUI() {
	//_gui.setScheme("TaharezLook.scheme");
	_gui.setFont("DejaVuSans-10");
	_gui.setMouseCursor("AlfiskoSkin/MouseArrow");
}

void AppScreen::updateInput() {
	SDL_Event input;
	while (SDL_PollEvent(&input)) {
		_gui.onSDLEvent(input);
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
				_scene["Cuboid"]->physics()->addImpulse(glm::vec3{ 100.0f, 100.0f, 100.0f });
				break;
			case SDLK_t:
				//_scene["Wall"]->transform.rotateTo(0.0f, glm::vec3{ 0.0f, 0.0f, 1.0f });
				break;
				/* DEBUG: Control the cube :*/
			case SDLK_KP_8:
				//_btBodies["Cuboid"]->applyCentralImpulse(btVector3{ 0.0f, 0.0f, 100.0f });
				break;
			case SDLK_KP_2:
				//_btBodies["Cuboid"]->applyCentralImpulse(btVector3{ 0.0f, 0.0f, -100.0f });
				break;
			case SDLK_KP_4:
				//_btBodies["Cuboid"]->applyCentralImpulse(btVector3{ -100.0f, 0.0f, 0.0f });
				break;
			case SDLK_KP_6:
				//_btBodies["Cuboid"]->applyCentralImpulse(btVector3{ 100.0f, 0.0f, 0.0f });
				break;
			case SDLK_KP_7:
				//_btBodies["Cuboid"]->applyCentralImpulse(btVector3{ 0.0f, 100.0f, 0.0f });
				break;
			case SDLK_KP_1:
				//_btBodies["Cuboid"]->applyCentralImpulse(btVector3{ 0.0f, -100.0f, 0.0f });
				break;
			case SDLK_KP_9:
				//_btBodies["Cuboid"]->applyTorque(btVector3{ 0.0f, 10.0f, 0.0f });
				break;
			case SDLK_KP_3:
				//_btBodies["Cuboid"]->applyTorque(btVector3{ 0.0f, -10.0f, 0.0f });
				break;
			case SDLK_KP_5:
				//_scene["Cuboid"]->transform.setWorldMatrix(glm::mat4{});
				break;
			case SDLK_SPACE:
				addCube(10.0f, _cam.pos());
				break;
			case SDLK_F1:
				_console.setVisible(_console.isVisible() ? false : true);
				break;
				//debug:
			case SDLK_F2:
				_scene.switchCull();
				break;
			case SDLK_g:
				_drender.gramSchmidt();
				break;
				break;
			case SDLK_F3:
			{
				pLight* light = _scene.findPLight("plight");
				glm::vec3 pos = light->pos();
				light->setProperty(lightProps::pos, glm::vec3{ pos.x + 1.0f, pos.y + 1.0f, pos.z + 1.0f });
			}
			break;
			case SDLK_F4:
			{
				pLight* light = _scene.findPLight("plight");
				glm::vec3 pos = light->pos();
				light->setProperty(lightProps::pos, glm::vec3{ pos.x - 1.0f, pos.y - 1.0f, pos.z - 1.0f });
			}
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
			_cam.rotate(dx, dy);
			break;
		case SDL_MOUSEWHEEL:
			_cam.zoom(input.wheel.y);
			break;
		case SDL_MOUSEBUTTONDOWN:
			switch (input.button.button) {
			case SDL_BUTTON_LEFT:
			{
				/* get the world space position of the mouse: */
				std::cout << "left\n";
				glm::vec3 rayS = _cam.pos();
				glm::vec3 rayDir = _cam.dir();

				DebugInfo::instance()->addStaticLine(glm::vec4{ rayS, 0.1f }, glm::vec4{ rayDir * 100.0f , 0.1f });
				Physics::instance()->rayPick(rayS, rayDir);
			}
				break;
			case SDL_BUTTON_RIGHT:
				std::cout << "right\n";
				break;
			}
		}
	}
}

void AppScreen::addCube(float mass, const glm::vec3 pos) {
	static int i = 0;
	std::string name = "Cube" + std::to_string(i++);

	glm::vec3 v = _cam.dir() * 10.0f;

	_scene.addChild(new Cuboid{ "xml/cube.xml" }, pos, name);
	_scene[name]->transform.setScale(glm::vec3{ 1.0f, 2.0f, 0.5f });
	_scene[name]->addPhysics(BodyType::cuboid, 10.0f, v);
}