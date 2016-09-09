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

namespace vitiGL {

class Frustum;

/*	CLASS SceneNode ------------------------------------------------------------------------------ */
class SceneNode {
public:
	SceneNode(IDrawable* drawable = nullptr, glm::vec3 pos = { 0.0f, 0.0f, 0.0f });
	virtual ~SceneNode();
	
	virtual void update(const Uint32& deltaTime);

	virtual void draw(const Shader& shader) const;
	virtual void drawAll(const Shader& shader) const;

	virtual void addChild(SceneNode* s);

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

protected:
	IDrawable*	_drawable;

	SceneNode*	_parent;
	std::vector<SceneNode*> _children;

	glm::mat4	_M;		//model position
	glm::mat4	_W;		//world position

};

/*	CLASS ShapeNode ------------------------------------------------------------------------------ */
class ShapeNode  {
public:
	ShapeNode(Shape* shape = nullptr, glm::vec3 pos = { 0.0f, 0.0f, 0.0f }, float radius = 1.0f);
	virtual ~ShapeNode();

	virtual void	drawNaked(const Shader& shader) const;
	virtual void	drawAllNaked(const Shader& shader) const;

	void			setShape(Shape* s)			{ _drawable = s; }

	/* getters and setters for the bounding sphere: */
	void		setRadius(float r)				{ _radius = r; }
	float		radius() const					{ return _radius; }
	void		setDistance(float d)			{ _distance = d; }
	float		distance() const				{ return _distance; }

	/* get an iterator for the nodes children: */
	auto		childrenBegin()	const			{ return _children.begin(); }
	auto		childrenEnd() const				{ return _children.end(); }

	/* static function to compare the distance to the camera between two nodes: */
	static bool compareDistance(ShapeNode* a, ShapeNode* b) {
		return (a->_distance < b->_distance ? true : false);
	}

private:
	float		_distance;	/* distance to the camera */
	float		_radius;	/* radius of the bounding sphere */
	Shape*		_shape;
};

/*	CLASS LightNode ------------------------------------------------------------------------------ */
class LightNode : public SceneNode {
public:

private:
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

	void addChild(IDrawable* drawable, glm::vec3 pos, float radius, 
				  const std::string& name = "", 
				  const std::string& parentName = "root");

	/* access scene elements: */
	SceneNode* findByName(const std::string& name);
	
	void update(const Uint32& deltaTime);

	/* draw everything in the scene: */
	void draw(const Shader& shader);
	/* draw the scene with view-frustum culling: */
	void drawCulled(const Shader& shader, Frustum& frustum);

	/* draw everything, but without textures (->for the shadowmap) */
	void drawAllNaked(const Shader& shader) const;
	void drawAllNakedCulled(const Shader& shader, Frustum& frustum);

	/* draw lights (for second pass in deferred rendering) */
	void drawDLights(const Shader& shader) const;
	void drawPlights(const Shader& shader) const;

	/* allow indexing the scene: */
	SceneNode* operator [] (const std::string& nodeName) {
		SceneNode* node = findByName(nodeName);
		if (node == nullptr) throw vitiError{ ("Invalid index in scene[" + nodeName + "]").c_str() };
		return node;
	}

	//debug - switch front and back face for point light drawing:
	void switchCull() {
		for (auto& L : _plights) {
			L->_front = (L->_front)? false : true;
		}
	}

private:
	void updateCullingList(Frustum& frustum, SceneNode* from);

	int _counter;

	std::vector<SceneNode*> _cullingList;

	SceneNode* _root; //we store root so we dont need to search it

	/* seperate lists for drawing: */
	std::map<std::string, SceneNode*> _scene;
	std::vector<Shape*>		_shapes;
	std::vector<dLight*>	_dlights;
	std::vector<pLight*>	_plights;
};

}
