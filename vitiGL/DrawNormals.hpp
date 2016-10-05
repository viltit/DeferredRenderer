/*	small class to draw the per-vertex normals of every object in a scene 
	use it AFTER you drawed the scene normally							  */

#pragma once

#include "Shader.hpp"

namespace vitiGL {

class Scene;
class Camera;

class DrawNormals {
public:
	DrawNormals(Camera* camera, Scene* scene);
	~DrawNormals();

	void draw();

private:
	Scene*		_scene;
	Camera*		_cam;
	Shader		_shader;

};
}
