#include "Scene.hpp"

#include <iostream>

namespace vitiGL {

SceneNode::SceneNode(Shape* s, glm::vec3 pos, float radius)
	:	_parent		{ nullptr },
		_shape		{ s },
		_scale		{ glm::vec3{ 1.0f, 1.0f, 1.0f } },
		_radius		{ radius },
		_distance	{ 0.0f }
{
	setPos(pos);
}

SceneNode::~SceneNode(){
	if (_shape) delete _shape;
	_shape = nullptr;

	for (auto& C : _children)
		if (C != nullptr) delete C;
}

void SceneNode::update(const Uint32 & deltaTime) {
	/* calculate new position: */
	if (_parent) _W = _parent->_W * _M;
	else _W = _M;
	
	/* give world position to the shape for drawing: */
	if (_shape) {
		_shape->setPosMatrix(_W);
		_shape->rotate(glm::radians(float(deltaTime)), glm::vec3{ 0.0f, 1.0f, 0.0f });
	}

	/* update all children: */
	for (auto& C : _children) C->update(deltaTime);
}

void SceneNode::draw(const Shader & shader) const {
	if (_shape) _shape->draw(shader);
}

void SceneNode::drawNaked(const Shader & shader) const {
	if (_shape) _shape->drawNaked(shader);
}

void SceneNode::drawAll(const Shader & shader) const {
	if (_shape) _shape->draw(shader);
	for (auto& C : _children) C->drawAll(shader);
}

void SceneNode::drawAllNaked(const Shader & shader) const {
	if (_shape) _shape->drawNaked(shader);
	for (auto& C : _children) C->drawAllNaked(shader);
}

void SceneNode::addChild(SceneNode * s) {
	_children.push_back(s);
	s->_parent = this;
}



}