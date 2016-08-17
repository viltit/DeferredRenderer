#include "Frame.hpp"
#include "vitiGL.hpp"

#include "Timer.hpp"

#include <glm/gtc/type_ptr.hpp>

using namespace vitiGL;

Frame::Frame() 
	:	appState	{ AppState::run },
		window		{ "OpenGL", 600, 600, wStyle::max },
		cam			{ float(window.width()) / float(window.height()) },
		gui			{ "GUI" },
		renderer	{ &window, &scene, &cam }
{
	init();
	loop();
}


Frame::~Frame() {
}

void Frame::init() {

	for (int i = -5; i < 4; i++) {
		std::string parentName = "Cube" + std::to_string(i);
		std::string childName = "Cuboid" + std::to_string(i);
		scene.addChild(new Cuboid{ "xml/cube.xml" }, glm::vec3{ i * 3.0, 2.0f, i * 3.0}, sqrt(2.0f), parentName);
		scene.addChild(new Cuboid{ "xml/cubeSmall.xml" }, glm::vec3{ 2.0, 0.0f, 2.0 }, sqrt(1.0f), childName, parentName);
		scene.addChild(new Cuboid{ "xml/cube_floor.xml" }, glm::vec3{ -3.0f, -1.0f, -3.0f }, sqrt(1800.0f), "Floor");
	}

	gui.setScheme("AlfiskoSkin.scheme");
	gui.setFont("DejaVuSans-10");
}

void Frame::loop() {
	Timer time;
	Uint32 accTime{ 0 };
	int loops{ 0 };
	time.on();

	//Prepare fps-Counter-Widget:
	auto* fpsCounter = static_cast<CEGUI::PushButton*>(
		gui.createWidget(glm::vec4{ 0.01f, 0.03f, 0.1f, 0.05f }, glm::vec4{}, "AlfiskoSkin/Button", "TestButton"));

	while (appState != AppState::quit) {
		Uint32 frameTime = time.frame_time();
		accTime += frameTime;
		loops++;
		if (loops >= 30) {
			int fps = 1000 * loops/ accTime;
			//std::cout << "FPS: " << fps << std::endl;
			loops = 0;
			accTime = 0;
			std::string text = "FPS: " + std::to_string(fps);
			fpsCounter->setText(CEGUI::String(text));
		}
		
		/* this should happen in a sceneNode-derived class on sceen::update: */
		for (int i = -5; i < 4; i++) {
			std::string parent = "Cube" + std::to_string(i);
			std::string child = "Cuboid" + std::to_string(i);

			auto temp = scene[parent];
			temp->rotate(float(frameTime) / (10.0f * i + 5), glm::vec3{ 0.0f, 1.0f, 0.0f });

			temp = scene[child];
			temp->rotate(float(frameTime) / (20.0f * i + 5), glm::vec3{ 0.0f, 1.0f, 0.0f });
		}
		window.clear();

		cam.update();
		scene.update(time.get_time());

		renderer.draw();
		gui.draw();
		updateInput();
		window.swap();
	}
}

void Frame::updateInput() {
	SDL_Event input;
	while (SDL_PollEvent(&input)) {
		switch (input.type) {
		case SDL_QUIT:
			appState = AppState::quit;
			break;
		case SDL_KEYDOWN:
			switch (input.key.keysym.sym) {
			case SDLK_ESCAPE:
				appState = AppState::quit;
				break;
			case SDLK_w:
				cam.move(Move::forward);
				break;
			case SDLK_s:
				cam.move(Move::backward);
				break;
			case SDLK_a:
				cam.move(Move::left);
				break;
			case SDLK_d:
				cam.move(Move::right);
				break;
			case SDLK_F1:
				scene["Cube0"]->scale(glm::vec3{ 1.0f, 1.5f, 1.0f });
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
			cam.rotate(dx, dy);
			break;
		case SDL_MOUSEWHEEL:
			cam.zoom(input.wheel.y);
			break;
		}
	}
}
