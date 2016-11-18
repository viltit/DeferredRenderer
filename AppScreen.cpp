#include "AppScreen.hpp"
#include "vitiGL.hpp"

#include "App.hpp"

#include <conio.h>

#include "vitiGEO/Ray.hpp"
#include "vitiGEO/AABB.hpp"
#include "vitiGL/AABBShape.hpp"
#include "vitiGEO/Constraint.hpp"
#include "vitiGL/RayTriangle.hpp"

using namespace vitiGL;
using namespace vitiGEO;

AppScreen::AppScreen(App* app, vitiGL::Window* window)
	: IAppScreen{ app },
	_cam{ float(window->width()) / float(window->height()) },
	_renderer{ window, &_scene, &_cam },
	_drender{ window, &_scene, &_cam },
	_gui{ "GUI", "TaharezLook.scheme" },
	_console{ this, &_gui, "layouts/console.layout"}
{
	_index = SCREEN_INDEX_APP;
	RayTriangle::start();

	/* Create the scene elements: 
	int prefix = 1;
	for (int i = -5; i < 4; i++) {
		prefix = (prefix == 1) ? -1 : 1;
		for (int j = 1; j < 5; j++) {
			std::string parentName = "Cube" + std::to_string(i) + "/" + std::to_string(j);
			std::string childName = "Cuboid" + std::to_string(i) + "/" + std::to_string(j);
			std::string child2Name = "SmallCuboid" + std::to_string(i) + "/" + std::to_string(j);
			std::string lightName = "plight" + std::to_string(i);

			_scene.addChild(new Octahedron{ "xml/cube.xml" }, glm::vec3{ prefix * i * 3.0, j * 3.0f, i * 3.0 }, sqrt(2.0f), parentName);
			_scene.addChild(new Cuboid{ "xml/cubeSmall.xml" }, glm::vec3{ 2.0, 0.0f, 2.0 }, sqrt(1.0f), childName, parentName);
			_scene.addChild(new Tetrahedron{ "xml/cubeTiny.xml" }, glm::vec3{ 0.0, prefix * 1.0f, 0.0 }, sqrt(1.0f), child2Name, childName);
		}
	}*/

	/*_scene.addChild(new Model{ "Models/Old House/Old House 2 3D Models.obj", &_cam, false }, "Shark");
	_scene["Shark"]->transform.scale(glm::vec3{ 0.05f, 0.05f, 0.05f });
	_scene["Shark"]->transform.setPos(glm::vec3{ -5.0f, 0.0f, -5.0f });*/
	//_scene["Shark"]->addPhysics(10.0f);
	//_scene["Shark"]->removePhysics();


	_scene.addChild(new Cuboid{ "xml/cube_floor.xml" }, glm::vec3{ -3.0f, -3.0f, -3.0f }, "Floor");
	_scene.addChild(new Cuboid{ "xml/cube_floor.xml" }, glm::vec3{ 20.0f, 7.0f, -3.0f }, "Wall");
	SceneNode* wall = _scene.findByName("Wall");
	wall->transform.rotate(90.0f, glm::vec3{ 0.0f, 0.0f, 1.0f });

	_scene.addChild(new dLight{ "dlight", glm::vec3{ 0.5f, -1.0f, -0.5f } }, "dlight");
	_scene.setShadowcaster("dlight");

	pLight* plight = new pLight{ &_cam };
	plight->setProperty(lightProps::pos, glm::vec3{ 0.0f, 30.0f, 0.0f });
	plight->setProperty(lightProps::diffuse, glm::vec3{ 10.0f, 5.0f, 0.0f });
	plight->setProperty(lightProps::specular, glm::vec3{ 20.0f, 10.0f, 0.0f });

	_scene.addChild(plight, "plight");
	_scene.setShadowcaster("plight");

	/* Skybox: */
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

	/* test: */
	_scene.addChild(new Cuboid{ "xml/cubeSmall.xml" }, glm::vec3{ 3.0, 3.0f, 15.0 }, "Octahedron");
	_scene["Octahedron"]->addPhysics(10.0f);

	_scene.addChild(new Cuboid{ "xml/cubeSmall.xml" }, glm::vec3{ 5.0, 3.0f, 15.0 }, "Octahedron2");
	_scene["Octahedron2"]->addPhysics(10.0f);

	_scene.addChild(new Cuboid{ "xml/cubeSmall.xml" }, glm::vec3{ 3.0, 3.0f, 16.0 }, "Octahedron3");
	_scene["Octahedron3"]->addPhysics(10.0f);

	/*
	DistanceConstraint* c = new DistanceConstraint {
		_scene["Octahedron"]->physics(),
		_scene["Octahedron2"]->physics(),
		glm::vec3{ 4.0f, 3.0f, 15.0f},
		glm::vec3{ 6.0f, 3.0f, 15.0f}
	};
	PhysicEngine::instance()->addConstraint(c);*/

	/*
	pLight* plight2 = new pLight{ &_cam };
	plight2->setProperty(lightProps::pos, glm::vec3{ 0.0f, 20.0f, 20.0f });
	plight2->setProperty(lightProps::diffuse, glm::vec3{ 10.0f, 5.0f, 0.0f });
	plight2->setProperty(lightProps::specular, glm::vec3{ 20.0f, 10.0f, 0.0f });

	_scene.addChild(plight2, "plight2");*/

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

	/* Infobox update: 
	_infoBox->deactivate();
	std::string fps = "FPS: " + std::to_string(_timer.fps());
	_fps->setText(CEGUI::String(fps));
	std::string gamma = "Gamma (keypad + and -): " + std::to_string(_drender.gamma());
	_gamma->setText(CEGUI::String(gamma));
	std::string exposure = "HDR-Exposure (keypad / and *): " + std::to_string(_drender.exposure());
	_hdr->setText(CEGUI::String(exposure));
	std::string glError = "GL-Error: " + std::to_string(glGetError());
	_glError->setText(CEGUI::String(glError));
	_infoBox->activate();*/

	std::string fps = "FPS: " + std::to_string(_timer.fps());
	_console.setText(fps);
	_gui.update(frameTime);

	/*
	for (int i = -5; i < 4; i++) {
		for (int j = 1; j < 5; j++) {
			std::string parent = "Cube" + std::to_string(i) + "/" + std::to_string(j);
			std::string child = "Cuboid" + std::to_string(i) + "/" + std::to_string(j);
			std::string child2 = "SmallCuboid" + std::to_string(i) + "/" + std::to_string(j);

			auto temp = _scene[parent];
			temp->rotate(float(frameTime) / (20.0f * i + 5), glm::vec3{ 0.0f, 1.0f, 0.0f });

			temp = _scene[child];
			temp->rotate(float(frameTime) * j / (10.0f * i + 5), glm::vec3{ 0.0f, 0.0f, 1.0f });

			temp = _scene[child2];
			temp->rotate(float(frameTime) * j/ (5.0f * i  + 5), glm::vec3{ 0.0f, 1.0f, 0.0f });
		}
	}*/
	/*
	Model* temp = static_cast<Model*>(_scene["Shark"]);
	if (_rotate) 
		temp->transform.rotate(float(frameTime) / (20.0f), glm::vec3{ 0.0f, 1.0f, 0.0f });*/

	_scene["Octahedron"]->physics()->setForce(glm::vec3{ 0.0f, 0.0f, 0.0f });

	/* update all components: */
	updateInput();
	PhysicEngine::instance()->update(frameTime);
	_scene.update(frameTime);
	_cam.update();
	_gui.update(frameTime);

}

void AppScreen::draw() {
	_drender.draw();
	PhysicEngine::instance()->drawDebug();
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
			case SDLK_UP:
				_scene["Octahedron"]->physics()->addForce(glm::vec3{ 0.0f, 0.0f, 1.0f });
				break;
			case SDLK_DOWN:
				_scene["Octahedron"]->physics()->addForce(glm::vec3{ 0.0f, 0.0f, -1.0f });
				break;
			case SDLK_LEFT:
				_scene["Octahedron"]->physics()->addForce(glm::vec3{ -1.0f, 0.0f, 0.0f });
				break;
			case SDLK_RIGHT:
				_scene["Octahedron"]->physics()->addForce(glm::vec3{ 1.0f, 0.0f, 0.0f });
				break;
			case SDLK_F1:
				_console.setVisible(_console.isVisible()? false : true);
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
					light->setProperty(lightProps::pos, glm::vec3{ pos.x - 1.0f, pos.y - 1.0f, pos.z -1.0f });
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
			case SDLK_r:
				_scene["Octahedron"]->physics()->setAngularVelocity(glm::vec3{ 0.1f, 0.1f, 0.1f });
				_scene["Octahedron3"]->physics()->setAngularVelocity(glm::vec3{ -0.1f, -0.1f, -0.1f });
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
		{
			/* TEMPORARY DEBUG CODE - TEST RAY-OBJECT COLLISION DETECTION 
			
				this is messy and just here to see if we get correct results */

			//_scene.addChild(new Cuboid{ "xml/cubeSmall.xml" }, _cam.pos() + 30.0f * _cam.dir(), sqrt(1.0f));

			/*
			vitiGEO::Ray ray{ _cam.pos(), _cam.dir() * 30.0f };

			Model* temp = static_cast<Model*>(_scene.findByName("Shark"));
			vitiGEO::AABB* aabb = temp->aabb().at(2);

			glm::vec3 intersection;
			float f{ 0.0f };

			if (aabb->rayIntersection(ray, intersection, f)) {*/
				//_scene.addChild(new Octahedron{ "xml/cube.xml" }, intersection, sqrt(2.0f));

				
				/* test for detailed collision with the mesh: 
				glm::vec3 tuv{};
				std::vector<glm::vec3> vertices = temp->vertices().at(2);
				for (int i = 0; i < vertices.size();) {
					std::vector<glm::vec3> triangle;
					for (int j = 0; j < 3; j++) {
						glm::vec4 vertex = { vertices[i].x, vertices[i].y, vertices[i].z, 1.0f };
						vertex = temp->posMatrix() * vertex;
						triangle.push_back(glm::vec3{ vertex.x, vertex.y, vertex.z });
						i++;
					}
					if (ray.rayTriangleIntersect(triangle, tuv)) {
						std::cout << "---------------------------- HIT THE MESH!\n";
						std::cout << "Ray delta factor: " << tuv.x << std::endl;
						std::cout << "UV: " << tuv.y << "/" << tuv.z << std::endl;

						glm::vec3 hitPoint = ray._origin + glm::normalize(ray._delta) * tuv.x;

						_scene.addChild(new Cuboid{ "xml/cubeSmall.xml" }, hitPoint, sqrt(1.0f));
					}
				}*/

				/* TEST: USE TRANSFORM FEEDBACK: 
				auto i = temp->childrenBegin() + 2;
				
				Mesh* mesh = static_cast<Mesh*>((*i)->obj());

				std::vector<glm::vec3> output;
				RayTriangle::update(mesh, &ray, output);
				std::cout << "GPU HIT DETECTION RESULTS ----------------------\n";
				for (int i = 0; i < output.size(); i++) {
					std::cout << "Factor t: " << output[i].x << std::endl;
					std::cout << "uv: " << output[i].y << "/" << output[i].z << std::endl;

					//test drawing: 
					glm::vec3 hitPoint = ray._origin + glm::normalize(ray._delta) * output[i].x;
					_scene.addChild(new Cuboid{ "xml/cubeSmall.xml" }, hitPoint);
				}
			}*/
			
			/* END OF DEBUG CODE */
			
		}
			break;
		}



	}
}