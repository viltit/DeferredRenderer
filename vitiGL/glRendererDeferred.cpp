#include "glRendererDeferred.hpp"

namespace vitiGL {

glRendererDeferred::glRendererDeferred(const Window* window, Scene* scene, Camera* camera)
	:	_window		{ window },
		_gshader	{ "Shaders/DeferredRenderer/geo.vert.glsl", "Shaders/DeferredRenderer/geo.frag.glsl" },
		_lshader	{ "Shaders/DeferredRenderer/light.vert.glsl", "Shaders/DeferredRenderer/light.frag.glsl" },
		_fshader	{ "Shaders/DeferredRenderer/final.vert.glsl", "Shaders/DeferredRenderer/final.frag.glsl" },
		_dshader	{ "Shaders/simple.vert.glsl", "Shaders/simple.frag.glsl" },
		_scene		{ scene },
		_camera		{ camera }
{
	if (_window == nullptr) throw initError("<glRendererDeferred::glRendererDeferred> Window is a nullptr");
	initGbuffer();
	initLbuffer();
}


glRendererDeferred::~glRendererDeferred()
{
}

void glRendererDeferred::update() {
}

void glRendererDeferred::draw() {
	CamInfo cam = _camera->getMatrizes();
	glm::mat4 VP = cam.P * cam.V;

	_frustum.update(VP);

	drawGeo();
	drawLight();
	drawFinal();
}

void glRendererDeferred::drawGeo() {
	glBindFramebuffer(GL_FRAMEBUFFER, _gbuffer);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_gshader.on();

	_camera->setUniforms(_gshader);
	_scene->drawCulled(_gshader, _frustum);

	_gshader.off();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	_quad.draw(_dshader, _tbo[color]);
}

void glRendererDeferred::drawLight()
{
}

void glRendererDeferred::drawFinal()
{
}

void glRendererDeferred::initGbuffer() {
	/* define our three color attachments: */
	GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 , GL_COLOR_ATTACHMENT2 };

	/* generate and bind the framebuffer: */
	glGenFramebuffers(1, &_gbuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, _gbuffer);

	/* generate textures for the geometry buffer: */
	int w = _window->width();
	int h = _window->height();

	_tbo[normal] = initTexture(textureType::float16, w, h);
	_tbo[position] = initTexture(textureType::float16, w, h);
	_tbo[color] = initTexture(textureType::color, w, h);
	_tbo[depht] = initTexture(textureType::depth, w, h);

	/* attach the textures to the geometry-buffer: */
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[0], GL_TEXTURE_2D, _tbo[position], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[1], GL_TEXTURE_2D, _tbo[normal], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[2], GL_TEXTURE_2D, _tbo[color], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _tbo[depht], 0);

	/* tell openGL that we will draw into all 3 color attachments: */
	glDrawBuffers(3, attachments);

	/* check if the geometry buffer is complete: */
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw initError("<Renderer::init_gbuffer>\t: Framebuffer not complete");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void glRendererDeferred::initLbuffer() {
	/* same procedure as above: */
	GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

	glGenFramebuffers(1, &_lbuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, _lbuffer);

	int w = _window->width();
	int h = _window->height();
	_tbo[diffuse] = initTexture(textureType::color, w, h);
	_tbo[specular] = initTexture(textureType::color, w, h);

	glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[0], GL_TEXTURE_2D, _tbo[diffuse], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[1], GL_TEXTURE_2D, _tbo[specular], 0);

	glDrawBuffers(2, attachments);

	/* check if the geometry buffer is complete: */
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw initError("<Renderer::init_gbuffer>\t: Framebuffer not complete");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


GLuint initTexture(textureType type, int w, int h) {
	GLuint tbo;
	glGenTextures(1, &tbo);
	glBindTexture(GL_TEXTURE_2D, tbo);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	switch (type) {
	case textureType::color:
		glTexImage2D(GL_TEXTURE_2D, 0,
					 GL_RGBA,
					 w, h, 0,
					 GL_RGBA,
					 GL_UNSIGNED_BYTE,
					nullptr);
		break;
	case textureType::float16:
		glTexImage2D(GL_TEXTURE_2D, 0,
					 GL_RGB16F,
					 w, h, 0,
					 GL_RGB,
					 GL_FLOAT,
					 nullptr);
		break;
	case textureType::depth:
		glTexImage2D(GL_TEXTURE_2D, 0,
					 GL_DEPTH_COMPONENT24,
					 w, h, 0,
					 GL_DEPTH_COMPONENT,
					 GL_UNSIGNED_BYTE,
					 nullptr);
		break;
	default:
		throw vitiError("<InitTexture>: Invalid textureType");
	}
	return tbo;
}

}