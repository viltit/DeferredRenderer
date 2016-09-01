#pragma once

#include "IAppScreen.hpp"
#include "vitiGL.hpp"

class App;
class vitiGL::Window;

class AppScreen : public vitiGL::IAppScreen {
public:
	AppScreen(App* app, vitiGL::Window* window);
	~AppScreen();

	// Inherited via IAppScreen
	virtual void onEntry() override;
	virtual void onExit() override;

	virtual void update() override;
	virtual void draw() override;

	virtual int next() const override;
	virtual int previous() const override;

	//get the latest screen content:
	GLuint	texture() const { return _drender.texture(); }

protected:
	void initGUI();
	void updateInput();

	vitiGL::Scene		_scene;
	vitiGL::Camera		_cam;
	//vitiGL::glRenderer  _renderer;
	vitiGL::glRendererDeferred _drender; //test
	vitiGL::GUI			_gui;

	vitiGL::Timer		_timer;

	CEGUI::PushButton*	_fpsInfo;
	CEGUI::PushButton*	_gamma;
};

