#include "AppScreen.hpp"
#include "vitiGL.hpp"

#include "App.hpp"

using namespace vitiGL;

AppScreen::AppScreen(App* app, vitiGL::Window* window) 
	:	IAppScreen	{ app },
		_cam		{ float(window->width()) / float(window->height()) },
		_renderer	{ window, &_scene, &_cam },
		_gui		{ "GUI"}
{}


AppScreen::~AppScreen() {
}

void AppScreen::onEntry() {
	for (int i = -5; i < 4; i++) {
		std::string parentName = "Cube" + std::to_string(i);
		std::string childName = "Cuboid" + std::to_string(i);
		std::string child2Name = "SmallCuboid" + std::to_string(i);
		_scene.addChild(new Octahedron{ "xml/cube.xml" }, glm::vec3{ i * 3.0, 2.0f, i * 3.0 }, sqrt(2.0f), parentName);
		_scene.addChild(new Cuboid{ "xml/cubeSmall.xml" }, glm::vec3{ 2.0, 0.0f, 2.0 }, sqrt(1.0f), childName, parentName);
		_scene.addChild(new Tetrahedron{ "xml/cubeTiny.xml" }, glm::vec3{ 0.0, 1.0f, 0.0 }, sqrt(1.0f), child2Name, childName);
		_scene.addChild(new Cuboid{ "xml/cube_floor.xml" }, glm::vec3{ -3.0f, -1.0f, -3.0f }, sqrt(1800.0f), "Floor");
	}

	initGUI();
	SDL_ShowCursor(0);

	_timer.on();
}

void AppScreen::onExit() {
}

void AppScreen::update() {
	Uint32 frameTime = _timer.frame_time();

	for (int i = -5; i < 4; i++) {
		std::string parent = "Cube" + std::to_string(i);
		std::string child = "Cuboid" + std::to_string(i);
		std::string child2 = "SmallCuboid" + std::to_string(i);

		auto temp = _scene[parent];
		temp->rotate(float(frameTime) / (20.0f * i + 5), glm::vec3{ 0.0f, 1.0f, 0.0f });

		temp = _scene[child];
		temp->rotate(float(frameTime) / (10.0f * i + 5), glm::vec3{ 0.0f, 0.0f, 1.0f });

		temp = _scene[child2];
		temp->rotate(float(frameTime) / (5.0f * i + 5), glm::vec3{ 0.0f, 1.0f, 0.0f });
	}

	updateInput();
	_scene.update(frameTime);
	_cam.update();
	_gui.update(frameTime);
}

void AppScreen::draw() {
	_renderer.draw();
	_gui.draw();
}

int AppScreen::next() const
{
	return 0;
}

int AppScreen::previous() const
{
	return 0;
}

void AppScreen::initGUI() {
	_gui.setScheme("AlfiskoSkin.scheme");
	_gui.setFont("DejaVuSans-10");
	_gui.setMouseCursor("AlfiskoSkin/MouseArrow");

	auto* exitButton = static_cast<CEGUI::PushButton*>(
		_gui.createWidget(glm::vec4{ 0.01f, 0.9f, 0.1f, 0.05f }, glm::vec4{}, "AlfiskoSkin/Button", "ExitButton"));
	exitButton->setText("Exit");

	/* Set Button events: */
	exitButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&AppScreen::onExitClicked, this));
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
				_state = ScreenState::exit;
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
				_scene["Cube0"]->scale(glm::vec3{ 1.0f, 1.5f, 1.0f });
				break;
				/*
				case SDLK_F2:
				framebuffer.setKernel(Kernel::blur);
				break;
				case SDLK_F3:
				framebuffer.setKernel(Kernel::sharpen);
				break;
				case SDLK_F4:
				framebuffer.setKernel(Kernel::hGradient);
				break;
				case SDLK_F5:
				framebuffer.setKernel(Kernel::vGradient);
				break;
				case SDLK_F6:
				framebuffer.setKernel(Kernel::emboss);
				break;
				case SDLK_F7:
				framebuffer.setKernel(Kernel::edges);
				break;
				case SDLK_F9:
				framebuffer.setKernel(Kernel::none);
				break;*/
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

bool AppScreen::onExitClicked(const CEGUI::EventArgs& e) {
	_state = ScreenState::exit;
	return true;
}