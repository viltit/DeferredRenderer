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
	// the one time we use a friend class (for the menu's callback functions):
	friend class MenuScreen;

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

	//get a pointer to the scene:
	vitiGL::Scene* scene() { return &_scene; }

protected:
	void updateInput();

	void updateFreezed();	//called from MenuScreen

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

	vitiGL::Timer		_timer;

	Fork				_fork;
};

