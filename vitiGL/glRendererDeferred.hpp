#pragma once

#include "Scene.hpp"
#include "Window.hpp"
#include "Camera.hpp"
#include "Frustum.hpp"
#include "sQuad.hpp"
#include "Light.hpp"
#include "Shadowmap.hpp"

/*	Deferred Renderer

	naming convention: g- for geometry, l- for lightning, f- for finalizing 
*/

namespace vitiGL {

enum class textureType {
	color,
	float16,
	depth
};

class glRendererDeferred {
public:
	glRendererDeferred(const Window* window, Scene* scene, Camera* camera);
	virtual ~glRendererDeferred();

	virtual void update();
	virtual void draw();

	GLuint texture() const { return _framebuffer.copyTexture(); } //to get the latest on-screen picture

protected:
	/* enum for indexing the texture attachments: */
	enum texureBuffer {
		normal,
		color,
		position,
		depht,
		diffuse,
		specular
	};

	/* initializing functions for the constructor: */
	void		initGbuffer();
	void		initLbuffer();

	/* the three drawing stages: */
	void		drawGeo();
	void		drawLight();
	void		drawFinal();

	/* The shaders for the three drawing stages: */
	Shader		_gshader;
	Shader		_lshader;
	Shader		_fshader;
	Shader		_dshader; //debug

	/* the texture buffers: */
	GLuint		_tbo[6];

	/* ...and the framebuffers: */
	GLuint		_gbuffer;
	GLuint		_lbuffer;

	Framebuffer _framebuffer;

	/* Scene and Camera: */
	Scene*		_scene;
	Camera*		_camera;
	const Window*	_window;

	dLight		_dlight;	//NEEDS TO GET PART OF THE SCENE
	DirShadowmap _dshadow;

	/* cameras view-perspective frustum: */
	Frustum		_frustum;

	/* screen sized quad to to draw into :*/
	sQuad		_quad;
	sQuad		_debug;
	sQuad		_debug2;
	sQuad		_debug3;
	sQuad		_debug4;

	glm::vec2	_texelSize;
};

/* helper functions that may be useful for other classes too: */
GLuint		initTexture(textureType type, int w, int h);

}