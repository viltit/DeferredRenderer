#pragma once

#include <Frustum.hpp>

#include "Shader.hpp"
#include "Shadowmap.hpp"

#include "IRenderer.hpp"

namespace vitiGL {

class Window;
class Scene;
class Camera;

class glRenderer : public IRenderer {
public:
	glRenderer(Window* window, Scene* scene, Camera* camera);
	virtual ~glRenderer();

	//inherited from IRenderer: 
	virtual void update() override;
	virtual void draw() override;

	GLuint texture() const { return _framebuffer.copyTexture(); } //to get the latest on-screen picture

protected:
	/* view Frustum: */
	vitiGEO::Frustum _frustum;

	/* Framebuffers and Shaders: */
	Framebuffer		_framebuffer;
	Shader			_mainShader;  //for drawing the scene in 3d
	Shader			_quadShader;  //for drawing the final image on a screen-sized quad
	Shader			_dShader;	  //for a debugging window

	sQuad			_quad;

	sQuad			_debug;
	sQuad			_debug2;
	sQuad			_debug3;
	sQuad			_debug4;

	bool			_drawDShadows;
};

}