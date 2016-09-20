/* Interface for different renderer classes */

#pragma once

namespace vitiGL {

class Window;
class Scene;
class Camera;

class IRenderer {
public:
	IRenderer(Window* window, Scene* scene, Camera* camera) 
		:	_scene{ scene },
			_camera{ camera },
			_window{ window }
	{};

	virtual ~IRenderer() {};

	virtual void update() = 0;
	virtual void draw() = 0;

protected:
	Scene*	_scene;
	Camera* _camera;
	Window* _window;
};
}
