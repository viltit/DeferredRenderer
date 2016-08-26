/*	partly following this tutorial: 
	https://research.ncl.ac.uk/game/mastersdegree/graphicsforgames/scenemanagement/ 
*/

/* TO DO: Add possibility to select scene node with scene["nodeName"] */

#pragma once

#include <SDL2\SDL.h>
#include <glm/gtc/matrix_transform.hpp>
#include <map>

#include "Shape.hpp"
#include "Error.hpp"

namespace vitiGL {

class Frustum;

class SceneNode {
public:
	SceneNode(Shape* shape = nullptr, glm::vec3 pos = { 0.0f, 0.0f, 0.0f }, float radius = 1.0f);
	virtual ~SceneNode();

	virtual void	update(const Uint32& deltaTime);

	/* draws only this node: */
	virtual void	draw(const Shader& shader) const;
	virtual void	drawNaked(const Shader& shader) const;

	/* draws all children: */
	virtual void	drawAll(const Shader& shader) const;
	virtual void	drawAllNaked(const Shader& shader) const;

	void addChild(SceneNode* s);

	/*	getters and setters, all inline: 
		rotation, scaling, positioning will always affect the children too!		*/
	void		move(const glm::vec3& pos)		{ _W = glm::translate(_W, pos); _M = glm::translate(_M, pos); }
	void		setPos(const glm::vec3& pos)	{ _M[3][0] = pos.x; _M[3][1] = pos.y; _M[3][2] = pos.z; }
	glm::vec3	pos() const						{ return glm::vec3{ _W[3][0], _W[3][1], _W[3][2] }; }
	glm::mat4	posMatrix() const				{ return _W; }

	void		rotate(float angle, const glm::vec3& axis) {
		_M = glm::rotate(_M, glm::radians(angle), axis);
	}

	void		scale(const glm::vec3& scale)	{ _M = glm::scale(_M, scale); }

	void		setShape(Shape* s)				{ _shape = s; }

	/* getters and setters for the bounding sphere: */
	void		setRadius(float r)				{ _radius = r; }
	float		radius() const					{ return _radius; }
	void		setDistance(float d)			{ _distance = d; }
	float		distance() const				{ return _distance; }

	/* get an iterator for the nodes children: */
	auto		childrenBegin()	const			{ return _children.begin(); }
	auto		childrenEnd() const				{ return _children.end(); }

	/* static function to compare the distance to the camera between two nodes: */
	static bool compareDistance(SceneNode* a, SceneNode* b) {
		return (a->_distance < b->_distance ? true : false);
	}

private:
	SceneNode*	_parent;
	std::vector<SceneNode*> _children;

	Shape*		_shape;

	glm::mat4	_W;			/* world position */
	glm::mat4	_M;			/* model position relative to its parent */
	glm::vec3	_scale;

	float		_distance;	/* distance to the camera */
	float		_radius;	/* radius of the bounding sphere */
};


/*	CLASS SCENE --------------------------------------------------------------------
Wrapper to identify Scene Nodes by name and adding childs to a parent by the
parent name
----------------------------------------------------------------------------------- */

class Scene {
public:
	Scene();
	~Scene();

	void addChild(Shape* s, glm::vec3 pos, float radius, 
				  const std::string& name = "", 
				  const std::string& parentName = "root");

	SceneNode* findByName(const std::string& name);
	
	void update(const Uint32& deltaTime);

	/* draw everything in the scene: */
	void draw(const Shader& shader);
	/* draw the scene with view-frustum culling: */
	void drawCulled(const Shader& shader, Frustum& frustum);

	/* draw everything, but without textures (->for the shadowmap) */
	void drawAllNaked(const Shader& shader) const;
	void drawAllNakedCulled(const Shader& shader, Frustum& frustum);

	/* allow indexing the scene: */
	SceneNode* operator [] (const std::string& nodeName) {
		SceneNode* node = findByName(nodeName);
		if (node == nullptr) throw vitiError{ ("Invalid index in scene[" + nodeName + "]").c_str() };
		return node;
	}

private:
	void updateCullingList(Frustum& frustum, SceneNode* from);

	int _counter;
	std::vector<SceneNode*> _cullingList;

	SceneNode* _root; //we store root so we dont need to search it

	std::map<std::string, SceneNode*> _scene;
};

}
