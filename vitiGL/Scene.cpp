#include "Scene.hpp"
#include "Frustum.hpp"
#include "vitiGlobals.hpp"

#include <iostream>

namespace vitiGL {

SceneNode::SceneNode(Shape* s, glm::vec3 pos, float radius)
	:	_parent		{ nullptr },
		_shape		{ s },
		_scale		{ glm::vec3{ 1.0f, 1.0f, 1.0f } },
		_radius		{ radius },
		_distance	{ 0.0f }
{
	/* get the initial position in case the Shape already has one: */
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
	if (_shape) _shape->setModelMatrix(_W);

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

/*	CLASS SCENE -------------------------------------------------------------------- */

Scene::Scene() 
	: _counter{ 0 }
{
	_root = new SceneNode{};
	std::string name = "root";
	_scene.insert(std::make_pair(name, _root));
}

Scene::~Scene() {
	//needs cleanup
}

void Scene::addChild(Shape* s, glm::vec3 pos, float radius, const std::string& name /*= ""*/, const std::string & parentName /* = "root" */) {
	/* if no name is given, create a unique one: */
	std::string nodeName = name;
	if (name == "" ) nodeName = "Node[" + std::to_string(++_counter) + "]";

#ifdef CONSOLE_LOG
	//std::cout << "<Scene::addChild>Added a Scene Node with the name " << nodeName << " and the parent " << parentName << std::endl;
#endif

	/* create the scene node and search for parent: */
	SceneNode* child = new SceneNode(s, pos, radius);
	SceneNode* parent = findByName(parentName);

	if (parent == nullptr) {
		delete child;
		throw initError(("<Scene::addChild> Could not find a parent with the name " + parentName).c_str());
	}

	/* add the child to its parent and also add it into our map:*/
	parent->addChild(child);
	_scene.insert(std::make_pair(nodeName, child));
}

void Scene::addDLight(dLight * l, const std::string & name) {
	std::string lightName = name;
	if (name == "") lightName = "Light[" + std::to_string(++_counter) + "]";

	_dlights.insert(std::make_pair(name, l));
}

void Scene::addPLight(pLight * p, const std::string & name) {
	std::string lightName = name;
	if (name == "") lightName = "Light[" + std::to_string(++_counter) + "]";

	_plights.insert(std::make_pair(name, p));
}

SceneNode * Scene::findByName(const std::string & name) {
	auto node = _scene.find(name);
	if (node == _scene.end()) return nullptr; //possibly dangerous!
	return node->second;
}

dLight * Scene::findDLight(const std::string & name) {
	auto light = _dlights.find(name);
	if (light == _dlights.end()) return nullptr;
	return light->second;
}

pLight * Scene::findPLight(const std::string & name) {
	auto light = _plights.find(name);
	if (light == _plights.end()) return nullptr;
	return light->second;
}

void Scene::update(const Uint32 & deltaTime) {
	_root->update(deltaTime);
}

void Scene::draw(const Shader & shader) {
	_root->drawAll(shader);
}

void Scene::drawCulled(const Shader & shader, Frustum& frustum) {
	_cullingList.clear();
	updateCullingList(frustum, _root);

	for (auto& N : _cullingList) N->draw(shader); 
}

void Scene::drawAllNaked(const Shader & shader) const {
	_root->drawAllNaked(shader);
}

void Scene::drawAllNakedCulled(const Shader & shader, Frustum& frustum) {
	_cullingList.clear();
	updateCullingList(frustum, _root);

	for (auto& N : _cullingList) N->drawNaked(shader);
}

void Scene::drawDLights(const Shader & shader) const {
	for (auto i = _dlights.begin(); i != _dlights.end(); i++) {
		if (i->second) i->second->draw(shader);
	}
}

void Scene::drawPlights(const Shader & shader, const glm::vec3& camPos) const {
	for (auto i = _plights.begin(); i != _plights.end(); i++) {
		if (i->second) i->second->draw(shader, camPos);
	}
}


void Scene::updateCullingList(Frustum & frustum, SceneNode* from) {
	
	if (frustum.isInside(*from)) _cullingList.push_back(from);

	/* visit all childs by recursion: */
	for (auto i = from->childrenBegin(); i < from->childrenEnd(); i++) updateCullingList(frustum, *i);
}

}