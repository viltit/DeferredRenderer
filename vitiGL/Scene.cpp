#include "Scene.hpp"
#include "Frustum.hpp"
#include "vitiGlobals.hpp"

#include <iostream>

namespace vitiGL {

/* CLASS SCENE NODE ------------------------------------------------------------- */

SceneNode::SceneNode(IDrawable* d, glm::vec3 pos)
	:	_parent		{ nullptr },
		_drawable	{ d }
{
	/* get the initial position in case the Shape already has one: */
	setPos(pos);
}

SceneNode::~SceneNode(){
	if (_drawable) delete _drawable;
	_drawable = nullptr;

	for (auto& C : _children)
		if (C != nullptr) delete C;
}

void SceneNode::update(const Uint32 & deltaTime) {
	/* calculate new position: */
	if (_parent) _W = _parent->_W * _M;
	else _W = _M;
	
	/* give world position to the shape for drawing: */
	if (_drawable) _drawable->setModelMatrix(_W);

	/* update all children: */
	for (auto& C : _children) C->update(deltaTime);
}

void SceneNode::draw(const Shader & shader) const {
	if (_drawable) _drawable->draw(shader);
}

void SceneNode::drawAll(const Shader & shader) const {
	if (_drawable) _drawable->draw(shader);
	for (auto& C : _children) C->drawAll(shader);
}

void SceneNode::addChild(SceneNode * s) {
	_children.push_back(s);
	s->_parent = this;
}

/*	CLASS SHAPE NODE ----------------------------------------------------------------- */
ShapeNode::ShapeNode(Shape* shape, glm::vec3 pos, float radius = 1.0f) 
	:	SceneNode(shape, pos),
		_radius{ radius },
		_distance{ 0.0f },
		_shape { shape }
{}

ShapeNode::~ShapeNode() 
{}

void ShapeNode::drawNaked(const Shader & shader) const {
	if (_shape) _shape->drawNaked(shader);
}

void ShapeNode::drawAllNaked(const Shader & shader) const {
	if (_shape) _drawable->draw(shader);
	for (auto& C : _children) C->drawAll(shader);
}





/* CLASS SCENE ----------------------------------------------------------------------- */
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

void Scene::addChild
(
	IDrawable* drawable, 
	glm::vec3 pos, 
	float radius, 
	const std::string& name /*= ""*/, 
	const std::string & parentName /* = "root" */
)
{
	/* if no name is given, create a unique one: */
	std::string nodeName = name;
	if (name == "" ) nodeName = "Node[" + std::to_string(++_counter) + "]";

#ifdef CONSOLE_LOG
	std::cout << "<Scene::addChild>Added a Scene Node with the name " << nodeName << " and the parent " << parentName << std::endl;
#endif

	/* create the scene node and search for parent: */
	SceneNode* parent = findByName(parentName);
	if (parent == nullptr) {
		throw initError(("<Scene::addChild> Could not find a parent with the name " + parentName).c_str());
	}
	SceneNode* child = new SceneNode(drawable, pos, radius);

	/* link the child to its parent: */
	parent->addChild(child);

	/* put the child in the scene map: */
	_scene.insert(std::make_pair(nodeName, child));

	/* put the child in the appropriate vector:*/
	switch (drawable->type()) {
	case ObjType::shape:
		_shapes.push_back(static_cast<Shape*>(drawable));
		break;
	case ObjType::dlight:
		_dlights.push_back(static_cast<dLight*>(drawable));
		break;
	case ObjType::plight:
		_plights.push_back(static_cast<pLight*>(drawable));
		break;
	default:
		throw vitiError("<Scene::addChild>Unknown Object type.");
	}
}


SceneNode * Scene::findByName(const std::string & name) {
	auto node = _scene.find(name);
	if (node == _scene.end()) return nullptr; //possibly dangerous!
	return node->second;
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

void Scene::drawPlights(const Shader & shader) const {
	for (auto i = _plights.begin(); i != _plights.end(); i++) {
		if (i->second) i->second->draw(shader);
	}
}

void Scene::updateCullingList(Frustum & frustum, SceneNode* from) {
	
	if (frustum.isInside(*from)) _cullingList.push_back(from);

	/* visit all childs by recursion: */
	for (auto i = from->childrenBegin(); i < from->childrenEnd(); i++) updateCullingList(frustum, *i);
}

}