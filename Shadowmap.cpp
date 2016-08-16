#include "Shadowmap.hpp"

#include "vitiGL.hpp"
#include "vitiGlobals.hpp"
#include "Cache.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

namespace vitiGL {

DirShadowmap::DirShadowmap(const Camera& cam, const dLight * light, int width, int height,
						   const std::string & vertexPath, const std::string & fragmentPath) 
	:	_w		{ width },
		_h		{ height },
		_shader { vertexPath, fragmentPath },
		_fshader{ "Shaders/shadowmapFinal.vert.glsl", "Shaders/shadowmapFinal.frag.glsl"},
		_dshader{ "Shaders/shadowmap_d.vert.glsl", "Shaders/shadowmap_d.frag.glsl" },
		_light	{ light },
		_framebuffer { width, height },
		_quad	{ 0.5f, 0.5f }
{
#ifdef CONSOLE_LOG
	std::cout << "Initializing directional Shadow Map...";
#endif

	/* calculate the cascades: */
	_cascadeEnd[0] = cam.nearPlane();
	_cascadeEnd[1] = 7.0f;  //cam.near() + (cam.far() - cam.near()) / 5.0f;
	_cascadeEnd[2] = 20.0f;  //cam.near() + (cam.far() - cam.near()) / 5.0f;
	_cascadeEnd[3] = 40.0f; //cam.near() + (cam.far() - cam.near()) / 2.0f;
	_cascadeEnd[4] = cam.farPlane();

	initFramebuffer();

	_framebuffer.setKernel(Kernel::gaussianBlur);
#ifdef CONSOLE_LOG
	std::cout << "\tfinished.\n";
#endif
}

DirShadowmap::~DirShadowmap() {
	//if (_fbo = 0) glDeleteFramebuffers(1, &_fbo);
	//if (_tbo = 0) glDeleteTextures(1, &_tbo);
}

void DirShadowmap::on() {
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo[0]);
	glViewport(0, 0, _w, _h);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_FRONT);		//to fight shadow acne

	glClear(GL_DEPTH_BUFFER_BIT);
}

void DirShadowmap::draw(const CamInfo& camera, const Scene* scene) {
	if (!_light || !scene) return;

	/* STEP ONE: Render scene from the viewpoint of the light: */
	updateMatrices2(camera);

	_shader.on();

	/* ...do this step for each cascade: */
	for (int i = 0; i < 4; i++) {
		glBindFramebuffer(GL_FRAMEBUFFER, _fbo[i]);
		glClear(GL_DEPTH_BUFFER_BIT);
		glUniformMatrix4fv(_shader.getUniform("T"), 1, GL_FALSE, glm::value_ptr(_O[i] * _V[i]));
		scene->drawAllNaked(_shader);
	}

	_shader.off();

	/* STEP TWO: Draw Scene with shadows in a black-and-white picture:  */

	_framebuffer.on();
	_fshader.on();

	glCullFace(GL_BACK);

	setUniforms(_fshader);
	glUniform3f(_fshader.getUniform("dlightDir"), _light->dir().x, _light->dir().y, _light->dir().z);
	glm::mat4 VP = camera.P * camera.V;
	glUniformMatrix4fv(_fshader.getUniform("VP"), 1, GL_FALSE, glm::value_ptr(VP));
	scene->drawAllNaked(_fshader);
	
	_fshader.off();
	_framebuffer.off();

	_dtbo = _framebuffer.texture();

	/* STEP 3: Downsample the black'n'white picture, THEN blur it, and upsample again */
}

void DirShadowmap::off() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glCullFace(GL_BACK);
}

void DirShadowmap::setUniforms(const Shader & shader) {
	std::string uniformName;
	for (int i = 0; i < 4; i++) {
		//Light Space Matrix:
		uniformName = "L[" + std::to_string(i) + ']';
		glUniformMatrix4fv(shader.getUniform(uniformName), 1, GL_FALSE, glm::value_ptr(_O[i] * _V[i]));

		//Cascades End Point:
		uniformName = "cascadeEnd[" + std::to_string(i) + ']';
		glUniform1f(shader.getUniform(uniformName), _cascadeD[i]);
	}

	/* bind the shadowmap textures:*/
	for (int i = 0; i < 4; i++) {
		uniformName = "shadowMap[" + std::to_string(i) + ']';
		glActiveTexture(GL_TEXTURE5 + i);
		glBindTexture(GL_TEXTURE_2D, _tbo[i]);
		glUniform1i(shader.getUniform(uniformName), 5 + i);
	} 
}

/* to improve: only use one framebuffer, attach all textures to it, and render into different render targets */
void DirShadowmap::initFramebuffer() {
	/* ceate and configure the texture object we draw the shadow map into: */
	for (int i = 0; i < 4; i++) {
		glGenTextures(1, &_tbo[i]);
		glBindTexture(GL_TEXTURE_2D, _tbo[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, _w, _h, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		/* clamp to border, so all pixels outside the depth map are considered to be in the light:*/
		float color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL); //write the smallest depht into the buffer

		/* create the framebuffer and attach the texture to it: */
		glGenFramebuffers(1, &_fbo[i]);
		glBindFramebuffer(GL_FRAMEBUFFER, _fbo[i]);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _tbo[i], 0);
		glDrawBuffer(GL_NONE); //we dont draw colors, only depth values
		glReadBuffer(GL_NONE);

		/* check if the framebuffer is ready to use: */
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			throw initError("<Shadowmap::Shadomap>\tCould not initialize Framebuffer.");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}


/* this function needs some serious improvments 
	-> calculation of camera frustum in world space is wrong
*/
void DirShadowmap::updatMatrices(const CamInfo& camera) {

	/* only update if the camera perspective matrix was updated: */
	if (!camera.updated) return;

	glm::vec4 corners[8] = {
		glm::vec4{ -1.0f, -1.0f, 1.0f, 1.0f },		glm::vec4{ -1.0f, -1.0f, -1.0f, 1.0f },
		glm::vec4{ -1.0f, 1.0f, 1.0f, 1.0f },		glm::vec4{ -1.0f, 1.0f, -1.0f, 1.0f },
		glm::vec4{ 1.0f, -1.0f, 1.0f, 1.0f },		glm::vec4{ 1.0f, -1.0f, -1.0f, 1.0f },
		glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f },		glm::vec4{ 1.0f, 1.0f, -1.0f, 1.0f }
	};

	const glm::mat4 iPV = glm::inverse(camera.P * camera.V);		/* inverse perspective-view-matrix */
	for (int i = 0; i < 8; i++) {
		corners[i] = iPV * corners[i];
		corners[i] /= corners[i].w;
	}

	glm::vec3 min = glm::vec3{ corners[0].x, corners[0].y, corners[0].z };
	glm::vec3 max = glm::vec3{ corners[0].x, corners[0].y, corners[0].z };
	for (int i = 0; i < 8; i++) {
		if (corners[i].x < min.x) min.x = corners[i].x;
		if (corners[i].x > max.x) max.x = corners[i].x;
		if (corners[i].y < min.y) min.y = corners[i].y;
		if (corners[i].y > max.y) max.y = corners[i].y;
		if (corners[i].z < min.z) min.z = corners[i].z;
		if (corners[i].z > max.z) max.z = corners[i].z;
	}

	/* average all corners to find the middle point: */
	glm::vec3 center{ 0.0f, 0.0f, 0.0f };
	for (int i = 0; i < 8; i++) {
		center.x += corners[i].x;
		center.y += corners[i].y;
		center.z += corners[i].z;
	}
	center /= 8.0f;

	/* find max distance center -> corner */
	float length = 0.0f;
	for (int i = 0; i < 8; i++) {
		if (glm::distance(center, glm::vec3{ corners[i].x, corners[i].y, corners[i].z }) > length) length = glm::distance(center, glm::vec3{ corners[i].x, corners[i].y, corners[i].z });
	}
	length /= 4.0f;

	/* do NOT adjust frustum every frame: 
	if (_l < length || _l > 2 * length) _l = length;

	_O = glm::mat4{};
	_V = glm::mat4{};
	_O = glm::ortho(-_l, _l, -_l, _l, -_l, _l);
	_V = glm::lookAt(camera.pos, camera.pos + _light->dir(), glm::vec3{ 0.0f, 1.0f, 0.0f });
	_L = _O * _V;*/
}

void DirShadowmap::updateMatrices2(const CamInfo& camera) {
	/*	calculate the frusta of all cascades 
		see http://alextardif.com/ShadowMapping.html for some background */
	
	/* only update if the camera perspective matrix was updated: */
	//if (!camera.updated) return;

	/* calculate three camera perspective matrices from the cascades: */
	for (int i = 0; i < 4; i++) {
		_P[i] = glm::perspective(glm::radians(camera.fov), camera.aspect, _cascadeEnd[i], _cascadeEnd[i + 1]);
	}

	for (int i = 0; i < 4; i++) {
		glm::vec3 frustumCorners[8] = {
			glm::vec3{ -1.0f, 1.0f, 0.0f },
			glm::vec3{ 1.0f, 1.0f, 0.0f },
			glm::vec3{ 1.0f, -1.0f, 0.0f },
			glm::vec3{ -1.0f, -1.0f, 0.0f },
			glm::vec3{ -1.0f, 1.0f, 1.0f },
			glm::vec3{ 1.0f, 1.0f, 1.0f },
			glm::vec3{ 1.0f, -1.0f, 1.0f },
			glm::vec3{ -1.0f, -1.0f, 1.0f },
		};

		// get the cameras view-perspective matrix for the cascade:
		glm::mat4 camVP = camera.V;
		camVP = _P[i] * camVP;

		// get the inverse of the view-perspective matrix:
		glm::mat4 invCamVP = glm::inverse(camVP);

		// we can now transform the view frustum corners into world space:
		for (int j = 0; j < 8; j++) {
			frustumCorners[j] =  TransformTransposed(frustumCorners[j], invCamVP);
		}

		//...and get the center of this split:
		glm::vec3 center;
		for (int j = 0; j < 8; j++) {
			center += frustumCorners[j];
		}
		center /= 8.0f;

		// take the farthest corners of the frustum to calculate a bounding radius:
		// (see the link at the top of this functions for why we work with a radius)
		float radius = glm::length(frustumCorners[0] - frustumCorners[6]) / 2.0f;
		radius *= 1.6f; //ugly hack

		// calculate texels per unit by dividing the shadow map size with twice the radius 
		// (ie the space the shadowmap covers):
		float texelsPerUnit = (float)_w / (radius * 2.0f);
		glm::mat4 S{};
		S = glm::scale(S, glm::vec3{ texelsPerUnit, texelsPerUnit, texelsPerUnit });

		// create a temporary look-At Matrix:
		glm::mat4 tempV = glm::lookAt(glm::vec3{ 0.0f, 0.0f, 0.0f }, -_light->dir(), glm::vec3{ 0.0f, 1.0f, 0.0f });
		tempV = S * tempV;
		glm::mat4 tempVinv = glm::inverse(tempV);

		// move the frustum center in texel-sized increments and then get it back to its original space:
		center = TransformTransposed(center, tempV);
		center.x = (float)floor(center.x);	//clamp to texel increment
		center.y = (float)floor(center.y);
		center =  TransformTransposed(center, tempVinv);			//back to its original space

		glm::vec3 center3 = { center.x, center.y, center.z };

		// create the eye by moving in the opposite direction of the light:
		glm::vec3 eye = center3 - _light->dir() * 2.0f * radius;

		// we are now ready to create the lights view and ortho matrix:
		_V[i] = glm::lookAt(eye, center3, glm::vec3{ 0.0f, 1.0f, 0.0f });

		// for the ortho matrix, we multiply the near and far plane by 5 to add room for outside 
		// objects casting a shadow:

		_O[i] = glm::ortho(-radius, radius, -radius, radius, -5.0f * radius, 5.0f * radius);

		radius /= 1.6f;
		_cascadeD[i] = _cascadeEnd[i] + 2.0f * radius;
	}
}

glm::vec3 DirShadowmap::TransformTransposed(const glm::vec3 &point, const glm::mat4& matrix)
{
	glm::vec3 result;
	glm::vec4 temp(point.x, point.y, point.z, 1.0f); //need a 4-part vector in order to multiply by a 4x4 matrix
	glm::vec4 temp2;

	temp2.x = temp.x * matrix[0][0] + temp.y * matrix[1][0] + temp.z * matrix[2][0] + temp.w * matrix[3][0];
	temp2.y = temp.x * matrix[0][1] + temp.y * matrix[1][1] + temp.z * matrix[2][1] + temp.w * matrix[3][1];
	temp2.z = temp.x * matrix[0][2] + temp.y * matrix[1][2] + temp.z * matrix[2][2] + temp.w * matrix[3][2];
	temp2.w = temp.x * matrix[0][3] + temp.y * matrix[1][3] + temp.z * matrix[2][3] + temp.w * matrix[3][3];

	result.x = temp2.x / temp2.w;	//view projection matrices make use of the W component
	result.y = temp2.y / temp2.w;
	result.z = temp2.z / temp2.w;

	return result;
}


void DirShadowmap::debug() {
	_quad.draw(_dshader, _dtbo);
}

}