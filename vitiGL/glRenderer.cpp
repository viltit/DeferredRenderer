#include "glRenderer.hpp"

#include "Error.hpp"
#include "GaussBlur.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace vitiGL {

glRenderer::glRenderer(const Window* window, Scene* scene, Camera* camera) 
	:	_scene		{ scene },
		_camera		{ camera },
		_mainShader { "Shaders/vertex.glsl", "Shaders/fragment.glsl" },
		_quadShader { "Shaders/final.vert.glsl", "Shaders/final.frag.glsl" },
		_dShader	{ "Shaders/simple.vert.glsl", "Shaders/simple.frag.glsl" },
		_framebuffer{ window->width(), window->height(), "Shaders/framebuffer.vert.glsl", "Shaders/framebuffer.frag.glsl" }, //dangerous!
		_dlight		{ "dlight" }, //needs to be part of the scene
		_dshadow	{ new DirShadowmap{ *camera, &_dlight }},
		_debugQuad  { QuadPos::topRight }
{
	_dlight.setUniforms(_mainShader);
	_dlight.setProperty(lightProps::dir, glm::vec3{ 0.0f, -1.0f, 1.f }, _mainShader);
	_plight.setUniforms(_mainShader);
	_plight.setProperty(lightProps::pos, glm::vec3{ 2.0f, 2.0f, 1.0f }, _mainShader);
}


glRenderer::~glRenderer() {
	if (_dshadow) {
		delete _dshadow;
		_dshadow = nullptr;
	}
}

void glRenderer::draw() {
	CamInfo cam = _camera->getMatrizes();
	glm::mat4 VP = cam.P * cam.V;

	/* draw dir light shadowmap: */
	if (_dshadow) {
		_dshadow->on();
		_dshadow->draw(cam, _scene, _frustum);
		_dshadow->off();
	}

	/* turn framebuffer and shader on: */
	_framebuffer.on();
	_framebuffer.setKernel(Kernel::none);
	_mainShader.on();

	_camera->setUniforms(_mainShader);
	_frustum.update(VP);

	/* draw the scene (to do: give option to turn frustum culling on and off) */
	_scene->drawCulled(_mainShader, _frustum);

	_mainShader.off();
	_framebuffer.off();

	/* draw the framebuffer texture onto the screen and mix with the shadow-image: */
	std::vector<GLuint> textures;
	std::vector<std::string> names;

	textures.push_back(_framebuffer.copyTexture());
	names.push_back("image");
	if (_dshadow) {
		textures.push_back(_dshadow->texture());
		names.push_back("shadowMap");
	}

	/* final on-screen image: */
	_framebuffer.on();

	_quad.draw(_quadShader, textures, names);

	_framebuffer.off();
	_framebuffer.draw();
	glDeleteTextures(1, &textures[0]);

	debug(_dshadow->texture());
}

void glRenderer::debug(GLuint texture) {
	_debugQuad.draw(_dShader, texture, "tex");
}

}