#include "glRendererDeferred.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "Scene.hpp"
#include "Camera.hpp"
#include "Window.hpp"
#include "Light.hpp"
#include "Model.hpp"
#include "glRendererDebug.hpp"

namespace vitiGL {

	glRendererDeferred::glRendererDeferred(Window* window, Scene* scene, Camera* camera)
		: IRenderer{ window, scene, camera },
		/* Shaders: */
		_gshader	{ "Shaders/DeferredRenderer/geo.vert.glsl", "Shaders/DeferredRenderer/geo.frag.glsl" },
		_lshader	{ "Shaders/DeferredRenderer/light.vert.glsl", "Shaders/DeferredRenderer/light.frag.glsl" },
		_fshader	{ "Shaders/DeferredRenderer/final.vert.glsl", "Shaders/DeferredRenderer/final.frag.glsl" },
		_dshader	{ "Shaders/simple.vert.glsl", "Shaders/simple.frag.glsl" },
		_sbshader	{ "Shaders/Skybox/skybox.vert.glsl", "Shaders/Skybox/skybox.frag.glsl" },
		_tshader	{ "Shaders/DeferredRenderer/vertex.glsl", "Shaders/DeferredRenderer/fragment.glsl" },
		/* Debug Windows: */
		_debug		{ QuadPos::topRight },
		_debug2		{ QuadPos::aboveMiddleRight },
		_debug3		{ QuadPos::belowMiddleRight },
		_debug4		{ QuadPos::bottomRight },
		/* Framebuffer and Gaussian Blur: */
		_framebuffer{ globals::window_w, globals::window_h,
						"Shaders/DeferredRenderer/pp.vert.glsl", "Shaders/DeferredRenderer/pp.frag.glsl" },
		_gauss{ _window->width() / 4, _window->height() / 4 },
		/* renderer Settings: */
		_gamma		{ 1.2f },
		_bloomTreshold{ 1.0f },
		_exposure	{ 0.4f },
		_normals	{ camera, scene },
		_glDrawMode { GL_FILL },
		_drawNormals { false },
		_drawDebugWin{ false },
		_drawDShadow { true },
		_drawPShadow { true },
		_applyBloom	 { true }
{
	if (_window == nullptr) throw initError("<glRendererDeferred::glRendererDeferred> Window is a nullptr");

	_texelSize.x = 1.0f / float(window->width());
	_texelSize.y = 1.0f / float(window->height());

	//set one-time-uniforms: (need adjustment on window-resize!):
	_lshader.on();
	glUniform2f(_lshader.getUniform("texelSize"), _texelSize.x, _texelSize.y); // to do: not needed every frame!
	_lshader.off();

	_framebuffer.setKernel(Kernel::sharpen);

	initGeoBuffer();
	initLightBuffer();
	initFinalBuffer();
}


glRendererDeferred::~glRendererDeferred() {
}

void glRendererDeferred::update() {
}

void glRendererDeferred::draw() {
	CamInfo cam = _camera->getMatrizes();
	glm::mat4 VP = cam.P * cam.V;

	_frustum.update(VP);

	/* draw shadowmaps: */
	if (_drawDShadow) _scene->drawDShadows(cam, _frustum);
	if (_drawPShadow) _scene->drawPShadows(cam);

	/* draw deferred passes: */
	drawGeo();
	drawLight();
	drawFinal();

	/* draw Skybox and transparent objects: */
	_framebuffer.on();
	_quad.draw(_dshader, _tbo[finalCol]);

	drawSkybox();
	drawTransparent();

	_framebuffer.off();

	/* apply bloom */
	drawBloom();

	/* draw the final image on the screen: */
	_framebuffer.draw();

	if (_drawDebugWin) {
		_debug.draw(_dshader, _scene->pShadowTex());
		_debug2.draw(_dshader, _scene->dShadowTex());
		_debug3.draw(_dshader, _tbo[color]);
		_debug4.draw(_dshader, _tbo[bloom]);
	}
}

void glRendererDeferred::gammaPlus(float value) {
	_gamma += value;
	Shader* s = _framebuffer.shader();
	s->on();
	glUniform1f(s->getUniform("gamma"), _gamma);
	s->off();
}

void glRendererDeferred::gammaMinus(float value) {
	_gamma -= value;
	Shader* s = _framebuffer.shader();
	s->on();
	glUniform1f(s->getUniform("gamma"), _gamma);
	s->off();
}

void glRendererDeferred::setGamma(float value) {
	_gamma = value;
	Shader* s = _framebuffer.shader();
	s->on();
	glUniform1f(s->getUniform("gamma"), _gamma);
	s->off();
}

void glRendererDeferred::setBloomTreshold(float value) {
	_fshader.on();
	_bloomTreshold = value;
	glUniform1f(_fshader.getUniform("treshold"), _bloomTreshold);
	_fshader.off();
}

void glRendererDeferred::setExposure(float value) {
	_exposure = value;
	Shader* s = _framebuffer.shader();
	s->on();
	glUniform1f(s->getUniform("exposure"), _exposure);
	s->off();
}

void glRendererDeferred::drawDShadow() {
	_drawDShadow = _drawDShadow ? false : true;
}

void glRendererDeferred::drawPShadow() {
	_drawPShadow = _drawPShadow ? false : true;
}

void glRendererDeferred::gramSchmidt() {
	_gramm = (_gramm)? false : true;
	_gshader.on();
	glUniform1i(_gshader.getUniform("gramm"), _gramm);
	_gshader.off();
}

void glRendererDeferred::drawGeo() {
	glBindFramebuffer(GL_FRAMEBUFFER, _buffer[geo]);

	glPolygonMode(GL_FRONT_AND_BACK, _glDrawMode);

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_gshader.on();
	_camera->setVPUniform(_gshader);
	_scene->drawShapes(_gshader, _frustum);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
	_gshader.off();

	if (_drawNormals) _normals.draw();
}

void glRendererDeferred::drawLight() {
	glBindFramebuffer(GL_FRAMEBUFFER, _buffer[light]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);	 //additive blending so multiple lights get brighter
	
	_lshader.on();

	/* set texture uniforms from geometry pass */
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, _tbo[normal]);
	glUniform1i(_lshader.getUniform("normal"), 4);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, _tbo[depht]);
	glUniform1i(_lshader.getUniform("depth"), 5);

	/* set view pos and texel size uniform: */
	glUniform3f(_lshader.getUniform("viewPos"), _camera->pos().x, _camera->pos().y, _camera->pos().z);

	/* DIRECTIONAL LIGHT: ------------------------------ */
	glm::mat4 M{};

	_lshader.on();

	/* set dshadow-texturemap: */
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, _scene->dShadowTex());
	glUniform1i(_lshader.getUniform("shadowmap"), 3);

	/* set subroutine uniforms: */
	std::vector<GLuint> subroutines;
	subroutines.push_back(glGetSubroutineIndex(_lshader.program(), GL_FRAGMENT_SHADER, "updateDlight"));
	if (_drawDShadow) {
		subroutines.push_back(glGetSubroutineIndex(_lshader.program(), GL_FRAGMENT_SHADER, "getDShadowOn"));
	}
	else {
		subroutines.push_back(glGetSubroutineIndex(_lshader.program(), GL_FRAGMENT_SHADER, "getDShadowOff"));
	}
	if (_drawPShadow) {
		subroutines.push_back(glGetSubroutineIndex(_lshader.program(), GL_FRAGMENT_SHADER, "getPShadowOn"));
	}
	else {
		subroutines.push_back(glGetSubroutineIndex(_lshader.program(), GL_FRAGMENT_SHADER, "getPShadowOff"));
	}
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 3, subroutines.data());

	/* set model and VP-Matrix uniform (which are both the identity matrix for directional lights) */
	glUniformMatrix4fv(_lshader.getUniform("M"), 1, GL_FALSE, glm::value_ptr(M));
	glUniformMatrix4fv(_lshader.getUniform("VP"), 1, GL_FALSE, glm::value_ptr(M));
	glUniformMatrix4fv(_lshader.getUniform("VPl"), 1, GL_FALSE, glm::value_ptr(M));

	_scene->drawDlights(_lshader);

	/* POINT LIGHTS: ------------------------------------- */
	/* override subroutine uniform: */
	subroutines.clear();
	subroutines[0] = glGetSubroutineIndex(_lshader.program(), GL_FRAGMENT_SHADER, "updatePlight");
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 3, subroutines.data());

	/* set view-perspective matrix: */
	_camera->setVPUniform(_lshader);

	/* set the shadowmap texture: */
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, _scene->pShadowTex());
	glUniform1i(_lshader.getUniform("shadowcube"), 3);

	/* draw the point lights: (more happening in the plight::draw() function!) */
	_scene->drawPlights(_lshader);

	_lshader.off();

	glCullFace(GL_BACK);
	glDisable(GL_BLEND);
}

void glRendererDeferred::drawSkybox() {
	CamInfo cam = _camera->getMatrizes();

	/* we need the depht information from the geometry buffer: */
	glBindFramebuffer(GL_READ_FRAMEBUFFER, _buffer[geo]);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _framebuffer.id());
	glBlitFramebuffer(0, 0, _window->width(), _window->height(), 0, 0, _window->width(), _window->height(),
		GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	
	_sbshader.on();
	glm::mat4 V = glm::mat4(glm::mat3(cam.V));
	glUniformMatrix4fv(_sbshader.getUniform("V"), 1, GL_FALSE, glm::value_ptr(V));
	glUniformMatrix4fv(_sbshader.getUniform("P"), 1, GL_FALSE, glm::value_ptr(cam.P));

	_scene->drawSkybox(_sbshader);

	_sbshader.off();
}

void glRendererDeferred::drawTransparent() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	CamInfo cam = _camera->getMatrizes();

	/* we need the depht information from the geometry buffer: */
	glBindFramebuffer(GL_READ_FRAMEBUFFER, _buffer[geo]);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _framebuffer.id());
	glBlitFramebuffer(0, 0, _window->width(), _window->height(), 0, 0, _window->width(), _window->height(),
		GL_DEPTH_BUFFER_BIT, GL_NEAREST);

	_tshader.on();

	glm::mat4 V = glm::mat4(glm::mat3(cam.V));
	glUniformMatrix4fv(_tshader.getUniform("VP"), 1, GL_FALSE, glm::value_ptr(cam.P * cam.V));

	_scene->drawTransparent(_tshader, _frustum);
	_tshader.off();

	/* TEMPRORARY DEBUG CODE: Visualize the aabb and their intersection
	Model* temp = static_cast<Model*>(_scene->findByName("Shark"));
	temp->drawAABB();*/

	glRendererDebug::instance()->draw(*_camera);

	/* debug: 
	try {
		AABBShape shape = AABBShape(_scene->findByName("Octahedron")->physics()->aabb(), _camera);
		shape.draw();
		AABBShape shape2 = AABBShape(_scene->findByName("Octahedron2")->physics()->aabb(), _camera);
		shape2.draw();
		AABBShape shape3 = AABBShape(_scene->findByName("Octahedron3")->physics()->aabb(), _camera);
		shape3.draw();
	}
	catch (...) {

	}*/
	
	/* END OF DEBUG CODE */

	glDisable(GL_BLEND);
}

void glRendererDeferred::drawFinal() {
	/* this pass draws the image and a seperate blurred image for bloom: */
	glBindFramebuffer(GL_FRAMEBUFFER, _buffer[final]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	_fshader.on();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _tbo[color]);
	glUniform1i(_fshader.getUniform("color"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _tbo[diffuse]);
	glUniform1i(_fshader.getUniform("diffuse"), 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, _tbo[specular]);
	glUniform1i(_fshader.getUniform("specular"), 2);
	
	_quad.drawNaked(_fshader);

	_fshader.off();

	if (_applyBloom) {
		_tbo[bloom] = _gauss.blurDS(_buffer[final], glm::ivec2{ _window->width(), _window->height() }, 4, 1);
	}

	glEnable(GL_DEPTH_TEST);
}

void glRendererDeferred::drawBloom() {
	Shader* s = _framebuffer.shader();
	s->on();
	if (_applyBloom) {
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, _tbo[bloom]);
		glUniform1i(s->getUniform("bloom"), 2);

		/* This did nnot work for some reason 
		GLuint uniform = glGetSubroutineIndex(s->program(), GL_FRAGMENT_SHADER, "bloomOn");
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &uniform);*/

		glUniform1i(s->getUniform("applyBloom"), 1);

	}
	else {
		/*
		GLuint uniform = glGetSubroutineIndex(s->program(), GL_FRAGMENT_SHADER, "blomOff");
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &uniform);*/

		glUniform1i(s->getUniform("applyBloom"), 0);
	}
	s->off();
}

void glRendererDeferred::initGeoBuffer() {
	/* define our three color attachments: */
	GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

	/* generate and bind the framebuffer: */
	glGenFramebuffers(1, &_buffer[geo]);
	glBindFramebuffer(GL_FRAMEBUFFER, _buffer[geo]);

	/* generate the textures for the different passes and attach them to the framebuffer: */
	int w = _window->width();
	int h = _window->height();

	_tbo[normal] = initTexture(textureType::float16, w, h);
	_tbo[color] = initTexture(textureType::color, w, h);
	_tbo[depht] = initTexture(textureType::depth, w, h);

	/* attach the textures to the framebuffer: */
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _tbo[normal], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _tbo[color], 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _tbo[depht], 0);

	/* tell OpenGL in what colorbuffers we draw: */
	glDrawBuffers(2, attachments);

	/* check if the geometry buffer is complete: */
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw initError("<Renderer::initGeoBuffer>\t: Framebuffer not complete");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void glRendererDeferred::initLightBuffer() {
	/* almost the same as above: */
	GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

	glGenFramebuffers(1, &_buffer[light]);
	glBindFramebuffer(GL_FRAMEBUFFER, _buffer[light]);

	int w = _window->width();
	int h = _window->height();
	_tbo[diffuse] = initTexture(textureType::float16, w, h);	//float16 for hdr
	_tbo[specular] = initTexture(textureType::float16, w, h);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _tbo[diffuse], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _tbo[specular], 0);

	glDrawBuffers(2, attachments);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw initError("<Renderer::initLightBuffer>\t: Framebuffer not complete");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void glRendererDeferred::initFinalBuffer() {

	GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

	glGenFramebuffers(1, &_buffer[final]);
	glBindFramebuffer(GL_FRAMEBUFFER, _buffer[final]);

	int w = _window->width();
	int h = _window->height();
	_tbo[finalCol] = initTexture(textureType::float16, w, h);
	_tbo[brightness] = initTexture(textureType::float16, w, h); //for bloom

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _tbo[finalCol], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _tbo[brightness], 0);

	glDrawBuffers(2, attachments);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw initError("<Renderer::initFinalBuffer>\t: Framebuffer not complete");

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
		glTexStorage2D	(GL_TEXTURE_2D, 1,
						GL_RGBA8,
						w, h);
		break;
	case textureType::float16:
		glTexStorage2D	(GL_TEXTURE_2D, 1,
						 GL_RGB16F,
						 w, h);
		break;
	case textureType::depth:
		glTexStorage2D	(GL_TEXTURE_2D, 1,
						GL_DEPTH_COMPONENT32,
						w, h);
		break;
	default:
		throw vitiError("<InitTexture>: Invalid textureType");
	}
	return tbo;
}

}
