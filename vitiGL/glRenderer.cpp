#include "glRenderer.hpp"

#include "Error.hpp"
#include "GaussBlur.hpp"
#include "Window.hpp"
#include "Camera.hpp"
#include "Scene.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace vitiGL {

glRenderer::glRenderer(Window* window, Scene* scene, Camera* camera) 
	:	IRenderer	{ window, scene, camera },
		_mainShader { "Shaders/Renderer/vertex.glsl", "Shaders/Renderer/fragment.glsl" },
		_quadShader { "Shaders/Renderer/final.vert.glsl", "Shaders/Renderer/final.frag.glsl" },
		_dShader	{ "Shaders/simple.vert.glsl", "Shaders/simple.frag.glsl" },
		_framebuffer{ window->width(), window->height(), "Shaders/framebuffer.vert.glsl", "Shaders/framebuffer.frag.glsl" }, //dangerous!
		_debugQuad  { QuadPos::topRight }
{}


glRenderer::~glRenderer() {
}

void glRenderer::update()
{}

void glRenderer::draw() {
	CamInfo cam = _camera->getMatrizes();
	glm::mat4 VP = cam.P * cam.V;

	_frustum.update(VP);

	/* draw shadowmaps: */
	_scene->drawDShadows(cam, _frustum);
	_scene->drawPShadows(cam);

	/* turn framebuffer and shader on: */
	_framebuffer.on();
	_framebuffer.setKernel(Kernel::none);
	_mainShader.on();

	/* set uniforms: */
	_camera->setUniforms(_mainShader);
	_scene->setLightningUniforms(_mainShader);

	/* draw the scene */
	_scene->drawShapes(_mainShader, _frustum);

	_mainShader.off();
	_framebuffer.off();

	/* get the shadowmap-textures: */
	std::vector<GLuint> textures;
	std::vector<std::string> names;
	textures.push_back(_framebuffer.texture());
	textures.push_back(_scene->dShadowTex());
	textures.push_back(_scene->pShadowTex());
	names.push_back("tex");
	names.push_back("dshadow");
	names.push_back("pshadow");

	/* final on-screen image: */
	_quad.draw(_quadShader, textures, names);
}
}