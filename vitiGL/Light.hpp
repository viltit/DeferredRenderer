/*	CLASS LIGHT --------------------------------------------------------------------------- 
	
	Task:	Hold all relevant information and methods for point lights and dir lights. 
			Also give a draw function for deferred rendering.

	Caveats: Light inherits from IDrawable

	To Do:	Add rim lights?
-------------------------------------------------------------------------------------------- */

#pragma once

#include <glm/glm.hpp>
#include <string>
#include "Shape.hpp"
#include "sQuad.hpp"

namespace vitiGL {

enum class lightProps {
	dir,
	pos,
	ambient,
	diffuse,
	specular,
	attenuation
};

/* some usefuel attenuation constants: */
namespace Attenuation {
	extern glm::vec3 r7;
	extern glm::vec3 r20;
	extern glm::vec3 r50;
	extern glm::vec3 r100;
	extern glm::vec3 r160;
	extern glm::vec3 r200;
}

class Camera;

class Light : public IGameObject {
public:
	Light(ObjType type) 
		:	IGameObject{ type }
	{}
	virtual ~Light() {}

	virtual void setProperty(lightProps property, const glm::vec3& value, const Shader& shader) = 0;
	virtual void setUniforms(const Shader& shader) const = 0;

protected:
};

/* Directional light ------------------------------------------------------------------------------ */
class dLight : public Light {
public:
	dLight	(const std::string& uniformName,
			 const glm::vec3& dir = { 0.0f, -1.0f, 0.0f }, 
			 const glm::vec3& ambient = { 0.1f, 0.1f, 0.1f },
			 const glm::vec3& diffuse = { 0.6f, 0.6f, 0.6f }, 
			 const glm::vec3& specular = { 1.0f, 1.0f, 1.0f });
	~dLight();

	void setProperty(lightProps property, const glm::vec3& value, const Shader& shader);
	void setUniforms(const Shader& shader) const override;

	/* inherited from IDrawable: */
	void draw(const Shader& shader) const override;

	/* getters and setters: */
	glm::vec3 dir() const	{ return _dir; }

private:
	std::string		_uniform;
	glm::vec3		_dir;
	glm::vec3		_ambient;
	glm::vec3		_diffuse;
	glm::vec3		_specular;
	sQuad			_quad;
};

/* Point Light ----------------------------------------------------------------------------------- */

/*	One PROBLEM here which stems from a bad design: Since plight inherits from IGameObject, it has
	a member _M. PLight also has a member sphere, which also inherits from IGameObject, and has
	its own _M. We always need to set boths _Ms in plights implementation.
*/

class pLight : public Light {
public:
	pLight( Camera* camera,
			const glm::vec3& pos = { 0.0f, 1.0f, 0.0f },
			const glm::vec3& ambient = { 0.1f, 0.1f, 0.1f },
			const glm::vec3& diffuse = { 0.6f, 0.6f, 0.6f },
			const glm::vec3& specular = { 1.0f, 1.0f, 1.0f },
			const glm::vec3& attenuation = { 1.0f, 0.09f, 0.032 },
			const std::string& uniformName = "plight");

	//~pLight();

	void setProperty(lightProps property, const glm::vec3& value, const Shader& shader);
	void setProperty(lightProps property, const glm::vec3& value);
	void setUniforms(const Shader& shader) const override;

	//inherited from IDrawable
	void draw(const Shader& shader) const override;

	void debugDraw(const Shader& shader) const;

	/* getters and setters: */
	float radius() const { return _r; }
	glm::vec3 pos() const { return _pos; }

	//override IGameObject-Method:
	virtual void setModelMatrix(const glm::mat4& M) override {
		//we only set the position part!
		_pos = { M[3][0], M[3][1], M[3][2] };
		_M[3][0] = M[3][0]; _M[3][1] = _M[3][1]; _M[3][2] = M[3][2];
		_sphere->setModelMatrix(_M);
	}

	//debug:
	bool _front;

private:
	/* calculate the lights radius based on the attenuation constants: */
	void calcRadius();

	std::string		_uniform;
	glm::vec3		_pos;
	glm::vec3		_attenuation;
	glm::vec3		_ambient;
	glm::vec3		_diffuse;
	glm::vec3		_specular;

	Sphere*			_sphere;
	float			_r;

	Camera*			_cam;
};

}
