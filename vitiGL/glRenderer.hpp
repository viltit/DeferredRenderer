#pragma once

#include "Scene.hpp"
#include "Window.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "Frustum.hpp"
#include "Shader.hpp"
#include "Shadowmap.hpp"

namespace vitiGL {

class glRenderer {
public:
	glRenderer(const Window* window, Scene* scene, Camera* camera);
	virtual ~glRenderer();

	virtual void draw();
	virtual void debug(GLuint texture);

	GLuint texture() const { return _framebuffer.copyTexture(); } //to get the latest on-screen picture

protected:
	/* Scene, Camera: */
	Scene*			_scene;
	Camera*			_camera;

	/* view Frustum: */
	Frustum			_frustum;

	/* Lights --> TO DO: Make lights part of the scene! */
	dLight			_dlight;
	pLight			_plight;
	DirShadowmap*	_dshadow;

	/* Framebuffers and Shaders: */
	Framebuffer		_framebuffer;
	Shader			_mainShader;  //for drawing the scene in 3d
	Shader			_quadShader;  //for drawing the final image on a screen-sized quad
	Shader			_dShader;	  //for a debugging window

	sQuad			_quad;
	sQuad			_debugQuad;

	bool			_drawDShadows;
};

}