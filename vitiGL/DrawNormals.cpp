#include "DrawNormals.hpp"

#include "Scene.hpp"
#include "Camera.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace vitiGL {

DrawNormals::DrawNormals(Camera* camera, Scene* scene)
	:	_scene	{ scene },
		_cam	{ camera },
		_shader	{ "Shaders/Effects/normal.vert.glsl", "Shaders/Effects/normal.frag.glsl", "Shaders/Effects/normal.geo.glsl" }
{}

DrawNormals::~DrawNormals() {
}

void DrawNormals::draw() {
	_shader.on();
	CamInfo cam = _cam->getMatrizes();
	glEnable(GL_DEPTH_TEST);
	glUniformMatrix4fv(_shader.getUniform("P"), 1, GL_FALSE, glm::value_ptr(cam.P));
	glUniformMatrix4fv(_shader.getUniform("V"), 1, GL_FALSE, glm::value_ptr(cam.V));
	_scene->drawShapesNaked(_shader);
	glDisable(GL_DEPTH_TEST);
	_shader.off();
}

}