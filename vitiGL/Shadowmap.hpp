/*	Classes for the shadow mapping of dynamic directional light and for dynamic point lights */

#pragma once

#include <GL/glew.h>
#include <string>
#include <map>

#include "Light.hpp"
#include "sQuad.hpp"
#include "Framebuffer.hpp"
#include "GaussBlur.hpp"

namespace vitiGL {

class Camera;
class Scene;
class Frustum;
struct CamInfo;

class DirShadowmap {
public:
	DirShadowmap	(const Camera& cam,
					const dLight* light = nullptr,
					int width = 1024, int height = 1024);
	~DirShadowmap	();

	/* make sure depth test and face culling is enabled! -> do it in on() function? */
	void on();
	void draw(const CamInfo & camera, Scene * scene, Frustum & frustum);
	void off();

	void setUniforms(const Shader& shader); //sets all the uniforms relevant to lightning

	void debug(); //draws the shadowmap onscreen

	//void multidraw();

	/* getters and setters are inline: */
	void		setLight(const dLight* light)	{ _light = light; }
	GLuint		texture() const { return _finalImg; }

	glm::vec3 DirShadowmap::TransformTransposed(const glm::vec3 &point, const glm::mat4& matrix);

protected:
	void		initFramebuffer();
	void		updatMatrices(const CamInfo& camera);	//wip -> adjust lights ortho matrix to "embrace" camera's view frustum
	void		updateMatrices2(const CamInfo& camera);	//TEST


	GLuint		_fbo[4];	// for the three cascades
	GLuint		_tbo[4];
	GLuint		_finalImg;  // final Shadow Image

	Framebuffer _framebuffer;  //for a black'n'white image of the scene with shadows

	float		_cascadeEnd[5];

	glm::mat4	_V[4];		/* view and ortho matrix of the shadowmap projection */
	glm::mat4	_O[4];
	glm::mat4	_P[4];		/* camera perspective matrix for each cascade */

	int			_w;
	int			_h;

	Shader		_shader;	//first pass: draw scene from the lights view
	Shader		_fshader;	//second pass: draw black and white scene

	GaussBlur	_gauss;		//to blur the shadowmap

	const dLight* _light;
};

/*	POINT SHADOWS ----------------------------------------------------------------------------------------- */

class PointShadowmap {
public:
	PointShadowmap	(int width = 1024, int height = 1024);
	~PointShadowmap();

	void on();
	void draw(const pLight* light, Scene* scene, const CamInfo& cam);
	void off();

	void addLight(const std::string& name, pLight* light);
	void removeLight(const std::string& name);

	void setUniforms(const Shader& shader);

	GLuint		texture() const { return _finalImg; }

protected:
	void		initFramebuffer();

	GLuint		_fbo;			// for the three cascades
	GLuint		_tbo;
	GLuint		_finalImg;		// final Shadow Image

	glm::mat4	_L[6];			//Light Space matrices

	Framebuffer _framebuffer;	//for a black'n'white image of the scene with shadows

	int			_w;
	int			_h;

	Shader		_shader;	//first pass: draw scene from the lights view
	Shader		_fshader;	//second pass: draw black and white scene

	GaussBlur	_gauss;		//to blur the shadowmap

	std::map<std::string, pLight*> _lights;
};

}
