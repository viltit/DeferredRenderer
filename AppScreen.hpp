#pragma once

#include "IAppScreen.hpp"
#include "vitiGL.hpp"
#include "Fork.hpp"

#include <string>
#include <map>



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
	//gui related: 
	void initGUI();
	void onPhysicsToggled();
	void onNormalsToggled();
	void onWireframeToggled();
	void onDebugWinToggled();
	void onPShadowToggled();
	void onDShadowToggled();
	void onBloomToggled();

	void updateInput();

	void addCube(float mass, const glm::vec3& pos);
	void addOctahedron(float mass, const glm::vec3& pos);
	void addTetrahedron(float mass, const glm::vec3& pos);
	void addIcosahedron(float mass, const glm::vec3& pos);

	void addChain(	const glm::vec3& startPos, 
					int counter, float distance, 
					const glm::vec3& scale = { 1.0f, 1.0f, 1.0f });

	vitiGL::Window*		_window;
	vitiGL::Scene		_scene;
	vitiGL::Camera		_cam;
	vitiGL::glRenderer  _renderer;
	vitiGL::glRendererDeferred _drender; //test

	vitiGL::GUI			_gui;

	vitiGL::Timer		_timer;

	Fork				_fork;

	bool				_rotate;
};

