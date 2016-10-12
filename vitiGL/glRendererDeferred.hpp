#pragma once

#include "Frustum.hpp"
#include "sQuad.hpp"
#include "Shadowmap.hpp"
#include "DrawNormals.hpp"

/*	Deferred Renderer

	naming convention: g- for geometry, l- for lightning, f- for finalizing 
*/

#include "IRenderer.hpp"

namespace vitiGL {

class Scene;
class Camera;
class Window;

enum class textureType {
	color,
	float16,
	depth
};

class glRendererDeferred : public IRenderer {
public:
	glRendererDeferred(Window* window, Scene* scene, Camera* camera);
	virtual ~glRendererDeferred();

	//inherited from IRenderer
	virtual void update() override;
	virtual void draw() override;

	GLuint texture() const { return _framebuffer.copyTexture(); } //to get the latest on-screen picture

	/* getters and setters: */
	void gammaPlus(float value);
	void gammaMinus(float value);
	void setGamma(float value);
	float gamma()			{ return _gamma; }

	void setBloomTreshold (float value);
	float bloomTreshold()	{ return _bloomTreshold;  }

	void setExposure(float value);
	float exposure() { return _exposure; }

	void setDrawMode() { _glDrawMode = (_glDrawMode == GL_FILL)? GL_LINE : GL_FILL; }

	void drawNormals(bool draw) { _drawNormals = draw;  }

	//debug:
	void gramSchmidt();

protected:
	/* enum for indexing the texture attachments: */
	enum texureBuffer {
		normal,
		color,
		diffuse,
		specular,
		finalCol,
		brightness,
		depht,
		bloom
	};

	enum frameBuffer {
		geo,
		light,
		final
	};

	/* initializing functions for the constructor: */
	void		initGeoBuffer();
	void		initLightBuffer();
	void		initFinalBuffer();

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
	GLuint		_tbo[8];

	/* ...and the framebuffers: */
	GLuint		_buffer[3];

	Framebuffer _framebuffer;

	DrawNormals _normals;

	/* Gaussian Blur for bloom: */
	GaussBlur	_gauss;

	/* cameras view-perspective frustum: */
	Frustum		_frustum;

	/* screen sized quad to to draw into :*/
	sQuad		_quad;
	sQuad		_debug;
	sQuad		_debug2;
	sQuad		_debug3;
	sQuad		_debug4;

	glm::vec2	_texelSize;

	float		_gamma;
	float		_bloomTreshold;
	float		_exposure;

	bool		_gramm;
	bool		_drawNormals;

	int			_glDrawMode;
};

/* helper functions that may be useful for other classes too: */
GLuint		initTexture(textureType type, int w, int h);

}