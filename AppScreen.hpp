#pragma once

#include "IAppScreen.hpp"
#include "vitiGL.hpp"

#include <string>

#include "vitiGEO/PhysicEngine.hpp"

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

	virtual std::string command(const std::string& command) override;

	//get the latest screen content:
	GLuint	texture() const { return _drender.texture(); }

protected:
	void initGUI();
	void updateInput();

	vitiGL::Scene		_scene;
	vitiGL::Camera		_cam;
	vitiGL::glRenderer  _renderer;
	vitiGL::glRendererDeferred _drender; //test

	vitiGEO::PhysicEngine _physics;

	vitiGL::GUI			_gui;
	vitiGL::Console		_console;

	vitiGL::Timer		_timer;

	bool				_rotate;
};

