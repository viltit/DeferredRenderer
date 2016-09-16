#include "Scene.hpp"
#include "Frustum.hpp"
#include "vitiGlobals.hpp"

#include <iostream>

namespace vitiGL {

/* CLASS SCENE NODE ------------------------------------------------------------- */

SceneNode::SceneNode(const std::string& name, IGameObject* object, glm::vec3 pos, float radius)
	:	_parent		{ nullptr },
		_obj		{ object },
		_radius		{ radius },
		_name		{ name }
{
	/* get the initial position in case the Shape already has one: */
	setPos(pos);
}

SceneNode::~SceneNode(){
	if (_obj) {
		delete _obj;
		_obj = nullptr;
	}

	for (auto& C : _children)
		if (C != nullptr) delete C;
}

void SceneNode::update(const Uint32 & deltaTime) {
	/* calculate new position: */
	if (_parent) _W = _parent->_W * _M;
	else _W = _M;
	
	/* give world position to the shape for drawing: */
	if (_obj) _obj->setModelMatrix(_W);

	/* update all children: */
	for (auto& C : _children) C->update(deltaTime);
}

void SceneNode::draw(const Shader & shader) const {
	if (_obj) _obj->draw(shader);
}

void SceneNode::drawNaked(const Shader & shader) const {
	if (_obj) _obj->drawNaked(shader);
}

void SceneNode::drawChilds(const Shader & shader) const {
	if (_obj) _obj->draw(shader);
	for (auto& C : _children) C->drawChilds(shader);
}

void SceneNode::addChild(SceneNode * s) {
	_children.push_back(s);
	s->_parent = this;
}


/* CLASS SCENE ----------------------------------------------------------------------- */
Scene::Scene() 
	: _counter{ 0 }
{
	std::string name = "root";
	_root = new SceneNode{name};
	_scene.insert(std::make_pair(name, _root));
}

Scene::~Scene() {
	//needs cleanup
}

void Scene::addChild
(
	IGameObject* object, 
	glm::vec3 pos, 
	float radius /* = 1.0f */, 
	const std::string& name /*= ""*/, 
	const std::string & parentName /* = "root" */
)
{
	/* if no name is given, create a unique one: */
	std::string nodeName = name;
	if (name == "" ) nodeName = "Node[" + std::to_string(++_counter) + "]";

#ifdef CONSOLE_LOG
	//std::cout << "<Scene::addChild>Added a Scene Node with the name " << nodeName << " and the parent " << parentName << std::endl;
#endif

	/* create the scene node and search for parent: */
	SceneNode* parent = findByName(parentName);
	if (parent == nullptr) {
		throw initError(("<Scene::addChild> Could not find a parent with the name " + parentName).c_str());
	}
	SceneNode* child = new SceneNode(nodeName, object, pos, radius);

	/* link the child to its parent: */
	parent->addChild(child);

	/* put the child in the scene map: */
	_scene.insert(std::make_pair(nodeName, child));

	/* put the child in the appropriate vector:*/
	switch (object->type()) {
	case ObjType::shape:
		_shapes.insert(std::make_pair(nodeName, static_cast<Shape*>(object)));
		break;
	case ObjType::dlight:
		_dlights.insert(std::make_pair(nodeName, static_cast<dLight*>(object)));
		break;
	case ObjType::plight:
	{
		pLight* light = static_cast<pLight*>(object);
		child->setPos(light->pos());
		_plights.insert(std::make_pair(nodeName, light));
	}
		break;
	default:
		throw vitiError("<Scene::addChild>Unknown Object type.");
	}
}

void Scene::addChild(IGameObject * object, const std::string & name, const std::string parentName) {
	addChild(object, glm::vec3{}, 1.0f, name, parentName);
}


/* wip */
void Scene::remove(const std::string & name) {
	SceneNode* node = findByName(name);
	if (!node) 
		throw vitiError(("<Scene::remove>Trying to remove an inexisting scene node with the name " + name).c_str());
	

	for (auto i = node->childrenBegin(); i != node->childrenEnd(); i++) {
		delete *i;
	}

	_scene.erase(name);
	_shapes.erase(name);
}


SceneNode * Scene::findByName(const std::string & name) {
	auto node = _scene.find(name);
	if (node == _scene.end()) return nullptr; //possibly dangerous!
	return node->second;
}


void Scene::update(const Uint32 & deltaTime) {
	_root->update(deltaTime);
}

void Scene::drawShapes(const Shader & shader) {
	for (const auto& S : _shapes) S.second->draw(shader);
}

void Scene::drawShapes(const Shader & shader, Frustum& frustum) {
	_cullingList.clear();
	updateCullingList(frustum, _root);

	for (auto& N : _cullingList) N->draw(shader); 
}

void Scene::drawShapesNaked(const Shader & shader) const {
	for (const auto& S : _shapes) S.second->drawNaked(shader);
}

void Scene::drawShapesNaked(const Shader & shader, Frustum& frustum) {
	_cullingList.clear();
	updateCullingList(frustum, _root);

	for (auto& N : _cullingList) N->drawNaked(shader);
}

void Scene::drawDLights(const Shader & shader) const {
	for (const auto& L : _dlights) L.second->draw(shader);
}

void Scene::drawPlights(const Shader & shader) const {
	for (const auto& L : _plights) L.second->draw(shader);
}

void Scene::setShadowcaster(const std::string& name) {
	SceneNode* node = findByName(name);
	if (node == nullptr) throw vitiError(("<Scene::setShadowcatser>Trying to add an inexisting pLight named" + name).c_str());
	if (node->type() == ObjType::plight) _shadowcaster = static_cast<pLight*>(node->obj());
}

void Scene::drawPShadows(const CamInfo & cam) {
	_pShadow.on();
	_pShadow.draw(_shadowcaster, this, cam);
	_pShadow.off();
}

dLight * Scene::findDLight(const std::string & name) {
	SceneNode* node = findByName(name);
	if (!node) return nullptr;
	return static_cast<dLight*>(node->obj());
}

pLight * Scene::findPLight(const std::string & name) {
	SceneNode* node = findByName(name);
	if (!node) return nullptr;
	return static_cast<pLight*>(node->obj());
}

void Scene::updateCullingList(Frustum & frustum, SceneNode* from) {
	if ((from->obj()) && from->type() == ObjType::shape) {
		if (frustum.isInside(*from)) _cullingList.push_back(from);
	}


	/* visit all childs by recursion: */
	for (auto i = from->childrenBegin(); i < from->childrenEnd(); i++) updateCullingList(frustum, *i);
}

}