#include "AppScreen.hpp"
#include "vitiGL.hpp"


#include "App.hpp"

using namespace vitiGL;

AppScreen::AppScreen(App* app, vitiGL::Window* window)
	: IAppScreen{ app },
	_cam{ float(window->width()) / float(window->height()) },
	_renderer	{ window, &_scene, &_cam },
	_drender{ window, &_scene, &_cam},
	_gui{ "GUI" }
{
	_index = SCREEN_INDEX_APP;

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

	
	_scene.addChild(new Octahedron{ "xml/cube.xml" }, glm::vec3{ 0.0, 10.0f, 0.0 }, sqrt(2.0f), "Octahedron1");
/*
	_scene.addChild(new Octahedron{ "xml/cube.xml" }, glm::vec3{ 3.0, 3.0f, 3.0 }, sqrt(2.0f), "Octahedron");
	_scene.remove("Octahedron");*/

	_scene.addChild(new Model{ "Models/nanosuit/nanosuit.obj" }, glm::vec3{ 3.0f, 20.0f, 5.0f }, sqrt(12.0f), "Shark");

	_scene.addChild(new Cuboid{ "xml/cube_floor.xml" }, glm::vec3{ -3.0f, -3.0f, -3.0f }, sqrt(1800.0f), "Floor");
	_scene.addChild(new Cuboid{ "xml/cube_floor.xml" }, glm::vec3{ 20.0f, 7.0f, -3.0f }, sqrt(1800.0f), "Wall");
	SceneNode* wall = _scene.findByName("Wall");
	wall->rotate(90.0f, glm::vec3{ 0.0f, 0.0f, 1.0f });

	_scene.addChild(new dLight{ "dlight", glm::vec3{ 0.5f, -1.0f, -0.5f } }, "dlight");
	_scene.setShadowcaster("dlight");

	/**/
	pLight* plight = new pLight{ &_cam };
	plight->setProperty(lightProps::pos, glm::vec3{ 3.0f, 10.0f, 0.0f });
	plight->setProperty(lightProps::diffuse, glm::vec3{ 10.0f, 5.0f, 0.0f });
	plight->setProperty(lightProps::specular, glm::vec3{ 20.0f, 10.0f, 0.0f });

	_scene.addChild(plight, "plight");

	pLight* plight2 = new pLight{ &_cam };
	plight2->setProperty(lightProps::pos, glm::vec3{ -3.0f, 10.0f, 2.0f });
	plight2->setProperty(lightProps::diffuse, glm::vec3{ 10.0f, 5.0f, 0.0f });
	plight2->setProperty(lightProps::specular, glm::vec3{ 20.0f, 10.0f, 0.0f });

	_scene.addChild(plight2, "plight2");
	_scene.setShadowcaster("plight2");

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
	_fpsInfo->setText(CEGUI::String(fps));

	std::string gamma = "Gamma (keypad +/-): " + std::to_string(_drender.gamma());
	_gamma->setText(CEGUI::String(gamma));

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

	//auto temp = _scene["Octahedron"];
	//temp->rotate(float(frameTime) / (20.0f), glm::vec3{ 0.0f, 1.0f, 0.0f });
	auto temp = _scene["Shark"];
	temp->rotate(float(frameTime) / 20.0f, glm::vec3{ 0.0f, 1.0f, 0.0f });

	updateInput();
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

void AppScreen::initGUI() {
	_gui.setScheme("AlfiskoSkin.scheme");
	_gui.setFont("DejaVuSans-10");
	_gui.setMouseCursor("AlfiskoSkin/MouseArrow");

	/* FPS Info: */
	_fpsInfo = static_cast<CEGUI::PushButton*>(
		_gui.createWidget(glm::vec4{ 0.01f, 0.03f, 0.2f, 0.05f }, glm::vec4{}, "AlfiskoSkin/Button", "FPS"));
	_gamma = static_cast<CEGUI::PushButton*>(
		_gui.createWidget(glm::vec4{ 0.01f, 0.1f, 0.2f, 0.05f }, glm::vec4{}, "AlfiskoSkin/Button", "Gamma"));
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
			case SDLK_F1:
				_scene["Cube0/1"]->scale(glm::vec3{ 1.0f, 1.5f, 1.0f });
				break;	
			//debug:
			case SDLK_F2:
				_scene.switchCull();
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
					_drender.setBloomTreshold(_drender.bloomTreshold() + 0.1f);
				break;
				case SDLK_F6:
					_drender.setBloomTreshold(_drender.bloomTreshold() - 0.1f);
				break;
				/*case SDLK_F7:
				framebuffer.setKernel(Kernel::edges);
				break;
				case SDLK_F9:
				framebuffer.setKernel(Kernel::none);
				break;*/
			case SDLK_KP_PLUS:
				_drender.gammaPlus(0.1f);
				break;
			case SDLK_KP_MINUS:
				_drender.gammaMinus(0.1f);
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
		}
	}
}