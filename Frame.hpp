#pragma once

#include "vitiGL.hpp"

class Frame {
public:
	Frame();
	~Frame();

private:
	void loop();
	void init();
	void updateInput();

	vitiGL::Window		window;
	vitiGL::AppState	appState;

	vitiGL::Shader		shader;
	vitiGL::Shader		fshader;

	vitiGL::Scene		scene;
	vitiGL::dLight		light;
	vitiGL::pLight		light2;

	vitiGL::Camera		cam;

	vitiGL::Frustum		frustum;

	vitiGL::DirShadowmap shadowmap;

	vitiGL::sQuad		quad;

	vitiGL::Framebuffer framebuffer;

	vitiGL::GUI			gui;
};

