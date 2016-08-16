#include "Frame.hpp"
#include "vitiGL.hpp"

#include "Timer.hpp"

#include <glm/gtc/type_ptr.hpp>

using namespace vitiGL;

Frame::Frame() 
	:	window		{ "OpenGL", 600, 600, wStyle::max },
		appState	{ AppState::run },
		cam			{ float(window.width()) / float(window.height()) },
		shader		{ "Shaders/vertex.glsl", "Shaders/fragment.glsl" },
		fshader		{ "Shaders/final.vert.glsl", "Shaders/final.frag.glsl"},
		light		{ "dlight"},
		light2		{ "plight" },
		framebuffer { window.width(), window.height(), "Shaders/framebuffer.vert.glsl", "Shaders/framebuffer.frag.glsl" },
		shadowmap	{ cam, &light },
		gui			{ "GUI" }
{
	init();
	loop();
}


Frame::~Frame() {
}

void Frame::init() {

	light.setUniforms(shader);
	light.setProperty(lightProps::dir, glm::vec3{ 0.0f, -1.0f, 0.1f }, shader);
	light2.setUniforms(shader);
	light2.setProperty(lightProps::pos, glm::vec3{ 2.0f, 2.0f, 1.0f }, shader);
	for (int i = -5; i < 5; i++)
		for (int j = -5; j < 5; j++) {
			SceneNode* node = new SceneNode{ new Cuboid{ "xml/cube.xml" }, glm::vec3{ i*3.0, 2.0f, j*3.0 }, sqrt(2.0f) };
			scene.addChild(node);
			}
	SceneNode* node = new SceneNode{ new Cuboid{ "xml/cube_floor.xml" }, glm::vec3{ -3.0f, -1.0f, -3.0f }, sqrt(1800.0f) };
	scene.addChild(node);

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
		accTime += time.frame_time();
		loops++;
		if (loops >= 30) {
			int fps = 1000 * loops/ accTime;
			//std::cout << "FPS: " << fps << std::endl;
			loops = 0;
			accTime = 0;
			std::string text = "FPS: " + std::to_string(fps);
			fpsCounter->setText(CEGUI::String(text));
		}

		window.clear();

		cam.update();

		CamInfo cInfo = cam.getMatrizes();
		glm::mat4 VP = cInfo.P * cInfo.V;

		shadowmap.on();
		shadowmap.draw(cInfo, &scene);
		shadowmap.off();
		glViewport(0, 0, window.width(), window.height());

		framebuffer.on();
		shader.on();

		glUniformMatrix4fv(shader.getUniform("VP"), 1, GL_FALSE, glm::value_ptr(VP));
		glUniform3f(shader.getUniform("viewPos"), cInfo.pos.x, cInfo.pos.y, cInfo.pos.z);

		frustum.update(VP);
		scene.update(time.get_time());

		nodeList.clear();
		updateNodeList(&scene);

		for (auto& N : nodeList) N->draw(shader);

		shader.off();
		//shadowmap.debug();
		framebuffer.off();

		/**/
		std::vector<GLuint> textures;
		textures.push_back(framebuffer.texture());
		textures.push_back(shadowmap.texture());
		std::vector<std::string> names;
		names.push_back("image");
		names.push_back("shadowMap");

		quad.draw(fshader, textures, names);

		//shadowmap.debug();
		//framebuffer.draw();

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
				cam.setTarget(scene.pos());
				break;
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
				break;
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

void Frame::updateNodeList(SceneNode * from) {
	if (frustum.isInside(*from)) nodeList.push_back(from);

	/* visit all childs by recursion: */
	for (auto i = from->childrenBegin(); i < from->childrenEnd(); i++) updateNodeList(*i);
}
