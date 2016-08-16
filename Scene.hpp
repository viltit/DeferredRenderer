/*	partly following this tutorial: 
	https://research.ncl.ac.uk/game/mastersdegree/graphicsforgames/scenemanagement/ 
*/

#pragma once

#include <SDL2\SDL.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Shape.hpp"

namespace vitiGL {

class SceneNode {
public:
	SceneNode(Shape* shape = nullptr, glm::vec3 pos = { 0.0f, 0.0f, 0.0f }, float radius = 1.0f);
	~SceneNode();

	virtual void	update(const Uint32& deltaTime);

	/* draws only this node: */
	virtual void	draw(const Shader& shader) const;
	virtual void	drawNaked(const Shader& shader) const;

	/* draws all children: */
	virtual void	drawAll(const Shader& shader) const;
	virtual void	drawAllNaked(const Shader& shader) const;

	void addChild(SceneNode* s);

	/* getters and setters, all inline: */
	void		setPos(const glm::vec3& pos)	{ _W = glm::translate(_W, pos); _M = glm::translate(_M, pos); }
	glm::vec3	pos() const						{ return glm::vec3{ _W[3][0], _W[3][1], _W[3][2] }; }
	glm::mat4	posMatrix() const				{ return _W; }

	void		setShape(Shape* s)				{ _shape = s; }

	/* getters and setters for the bounding sphere: */
	void		setRadius(float r)				{ _radius = r; }
	float		radius() const					{ return _radius; }
	void		setDistance(float d)			{ _distance = d; }
	float		distance() const				{ return _distance; }

	/* get an iterator for the nodes children: */
	auto		childrenBegin()					{ return _children.begin(); }
	auto		childrenEnd()					{ return _children.end(); }

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
}
