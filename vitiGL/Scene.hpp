/*	CLASS SCENE_NODE -------------------------------------------------------------------------
	
	loosly following this tutorial:
	https://research.ncl.ac.uk/game/mastersdegree/graphicsforgames/scenemanagement/

	Task:	Hold the objects in our scene and establish a parent-child relationship between
			them. Also, together with class Scene, perform viewFrustum culling
------------------------------------------------------------------------------------------------ */


#pragma once

#include <SDL2\SDL.h>
#include <glm/gtc/matrix_transform.hpp>
#include <map>

#include "Shape.hpp"
#include "Error.hpp"
#include "Light.hpp"
#include "Shadowmap.hpp"

namespace vitiGL {

class Frustum;

/*	CLASS SceneNode ------------------------------------------------------------------------------ */
class SceneNode {
public:
	SceneNode(const std::string& name, 
			  IGameObject* object = nullptr, 
			  glm::vec3 pos = { 0.0f, 0.0f, 0.0f }, 
			  float radius = 1.0f);

	virtual ~SceneNode();
	
	virtual void update(const Uint32& deltaTime);

	virtual void draw(const Shader& shader) const;
	virtual void drawNaked(const Shader& shader) const;
	virtual void drawChilds(const Shader& shader) const;

	virtual void addChild(SceneNode* s);

	virtual void remove();

	/*	getters and setters, all inline:
	rotation, scaling, positioning will always affect the children too!		*/
	void		move(const glm::vec3& pos) { _W = glm::translate(_W, pos); _M = glm::translate(_M, pos); }
	void		setPos(const glm::vec3& pos) { _M[3][0] = pos.x; _M[3][1] = pos.y; _M[3][2] = pos.z; }

	glm::vec3	pos() const { return glm::vec3{ _W[3][0], _W[3][1], _W[3][2] }; }
	glm::mat4	posMatrix() const { return _W; }

	void		rotate(float angle, const glm::vec3& axis) {
		_M = glm::rotate(_M, glm::radians(angle), axis);
	}

	void		scale(const glm::vec3& scale) { _M = glm::scale(_M, scale); }

	float		radius() const { return _radius; }

	std::string name() { return _name; }

	ObjType		type() { return _obj->type(); }
	IGameObject* obj() { return _obj; }

	/* get an iterator to the childrens vector: */
	auto		childrenBegin() { return _children.begin(); }
	auto		childrenEnd()	{ return _children.end(); }

protected:
	IGameObject* _obj;

	SceneNode*	_parent;
	std::vector<SceneNode*> _children;

	glm::mat4	_M;		//model position
	glm::mat4	_W;		//world position

	float		_radius;

	std::string _name;
};

/*	CLASS SCENE ------------------------------------------------------------------------------
	Task:	Wrapper to identify Scene Nodes by name and adding childs to a parent by the
			parent name. Scene also holds all lights.

	Problems:	Ideally, the lights would be SceneNodes too (ie a light can circle around a 
				parent). But right now, dir- and pointLights are both held in a seperate list

				Merging the list would require the lights to inherit from Shape, which in turn
				would add not-needed methods to them
--------------------------------------------------------------------------------------------- */

class Scene {
public:
	Scene();
	~Scene();

	void addChild(IGameObject* object, 
				  glm::vec3 pos = {},
				  float radius =  1.0f, 
				  const std::string& name = "", 
				  const std::string& parentName = "root");

	void addChild(IGameObject* object,
				  const std::string& name = "",
				  const std::string parentName = "root");

	/* removes node and all its children: */
	void remove(const std::string& name);

	/* access scene elements: */
	SceneNode* findByName(const std::string& name);
	
	void update(const Uint32& deltaTime);

	/* draw every Shape in the scene: */
	void drawShapes(const Shader& shader);
	/* draw every Shape in the scene with view-frustum culling: */
	void drawShapes(const Shader& shader, Frustum& frustum);

	/* draw everything, but without textures (->for the shadowmap) */
	void drawShapesNaked(const Shader& shader) const;
	void drawShapesNaked(const Shader& shader, Frustum& frustum);

	/* draw lights (for second pass in deferred rendering) */
	void drawDlights(const Shader& shader) const;
	void drawPlights(const Shader& shader) const;

	/* draw shadowmaps: */
	void setShadowcaster(const std::string& name);
	void drawPShadows(const CamInfo& cam);
	void drawDShadows(const CamInfo& cam, Frustum& frustum);

	/* get the shadowmaps: */
	GLuint pShadowTex() { return _pShadow.texture(); }
	GLuint dShadowTex() { return _dShadow.texture(); }

	/* allow indexing the scene: */
	SceneNode* operator [] (const std::string& nodeName) {
		SceneNode* node = findByName(nodeName);
		if (node == nullptr) throw vitiError{ ("Invalid index in scene[" + nodeName + "]").c_str() };
		return node;
	}


	//debug - switch front and back face for point light drawing:
	void switchCull() {
		for (auto& L : _plights) {
			L.second->_front = (L.second->_front)? false : true;
		}
	}

	//temporary function, needs another solution:
	dLight* findDLight(const std::string& name);
	pLight* findPLight(const std::string& name);

private:
	void updateCullingList(Frustum& frustum, SceneNode* from);

	int _counter;

	SceneNode* _root; //we store root so we dont need to search it

	/* seperate lists for drawing: */
	std::map<std::string, SceneNode*> _scene;
	std::vector<SceneNode*>	_cullingList;
	std::map<std::string, Shape*>	_shapes;
	std::map<std::string, dLight*>	_dlights;
	std::map<std::string, pLight*>	_plights;

	pLight*		   _pshadowcaster;
	dLight*		   _dshadowcaster;
	PointShadowmap _pShadow;
	DirShadowmap   _dShadow;
};

}
