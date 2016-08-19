#pragma once

#include "vitiGL.hpp"

class Frame {
public:
	Frame();
	~Frame();

private:
	void loop();
	void init();
	void initGUI();
	void updateInput();

	/* Event Handlers: */
	bool onExitClicked(const CEGUI::EventArgs& e);

	vitiGL::AppState	appState;

	vitiGL::Window		window;
	vitiGL::Scene		scene;
	vitiGL::Camera		cam;
	vitiGL::glRenderer  renderer;

	vitiGL::GUI			gui;
};

