#include "Scene.hpp"
#include "Frustum.hpp"
#include "vitiGlobals.hpp"
#include "Skybox.hpp"
#include "Camera.hpp"
#include "Mesh.hpp"
#include "Model.hpp"


#include <iostream>
#include <assert.h>

namespace vitiGL {

/* CLASS SCENE NODE ------------------------------------------------------------- */

SceneNode::SceneNode(const std::string& name, IGameObject* object, glm::vec3 pos)
	:	_parent		{ nullptr },
		_obj		{ object },
		_name		{ name }
{
	/* get the initial position in case the Shape already has one: */
	transform.setPos(pos);

	/* if the attached object has an aabb, get its "radius": */
	if (_obj) {
		if (_obj->type() == ObjType::mesh || _obj->type() == ObjType::shape) {
			Shape* s = static_cast<Shape*>(_obj);
			_radius = s->getAABB()->diameter() * 0.5f;
		}
		else _radius = 1.0f;
	}
}


/* If a scene node gets deleted, all its children will be deleted too: */
SceneNode::~SceneNode() {

#ifdef CONSOLE_LOG
	std::cout << "<SceneNode>Deleting an object with the name " << _name << std::endl;
#endif

	if (_obj) {
		std::cout << "... deleting _obj...\n";
		delete _obj;
		_obj = nullptr;
	}
	if (_physics) {
		std::cout << "...deleting _physics...\n";
		delete _physics;
		_physics = nullptr;
	}
	for (auto& C : _children) {
		if (C) {
			delete C;
			C = nullptr;
		}
	}
}

void SceneNode::update(const Uint32 & deltaTime) {
	/* tell transform to adapt parents matrix, if there is a parent: */
	if (_parent)  transform.setWorldMatrix(_parent->transform.worldMatrix() * transform.localMatrix());
	else transform.setWorldMatrix(transform.localMatrix());

	/* give world position to the shape for drawing: */
	if (_obj) {
		_obj->setModelMatrix(transform.worldMatrix());
		
		/* update aabb: */
		if (_obj->type() == ObjType::shape || _obj->type() == ObjType::mesh) {
			vitiGEO::AABB* aabb = static_cast<Shape*>(_obj)->getAABB();
			aabb->transform(transform.worldMatrix());
		}
	}

	/* update all children: */
	for (auto& C : _children) C->update(deltaTime);
}

void SceneNode::draw(const Shader & shader) const {
	if (_obj) _obj->draw(shader);
#ifdef CONSOLE_LOG
	else {
		std::cout << "<SceneNode::Draw>SceneNode named " << _name << " has no drawable object attached\n";
	}
#endif
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

void SceneNode::remove() {
	/* tell the parent that the child is dead: */
	for (auto i = _parent->childrenBegin(); i != _parent->childrenEnd(); i++) {
		if (*i == this) {
			_parent->_children.erase(i);

#ifdef CONSOLE_LOG
			std::cout << "Deleting a child named " << _name << " from parent " << _parent->name() << std::endl;
#endif
			break;
		}
	}
}

void SceneNode::addPhysics(float mass) {
	/* only mesh and shape objects supported: */
	if (_obj && _obj->type() != ObjType::mesh && _obj->type() != ObjType::shape)
		return;

	/* we assume the object has an aabb: */
	Shape* s = static_cast<Shape*>(_obj);
	_physics = new vitiGEO::PhysicObject{ transform, s->getAABB(), mass };
}

void SceneNode::removePhysics() {
	_physics->remove();
	delete _physics;
	_physics = nullptr;
}

/* CLASS SCENE ----------------------------------------------------------------------- */
Scene::Scene() 
	: _counter{ 0 },
	  _pshadowcaster { nullptr },
	  _dshadowcaster { nullptr },
	  _skybox		 { nullptr }
{
	std::string name = "root";
	_root = new SceneNode{name};
	_scene.insert(std::make_pair(name, _root));
}

Scene::~Scene() {
	if (_root) {
		delete _root;
		_root = nullptr;    
	}
}

void Scene::addChild
(
	IGameObject* object, 
	glm::vec3 pos, 
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
	SceneNode* child = new SceneNode(nodeName, object, pos);

	/* link the child to its parent: */
	parent->addChild(child);

	/* put the child in the scene map: */
	_scene.insert(std::make_pair(nodeName, child));

	/* we can insert a null-object: */
	addToList(child);
}

void Scene::addChild(IGameObject * object, const std::string & name, const std::string& parentName) {
	addChild(object, glm::vec3{}, name, parentName);
}

void Scene::addChild(SceneNode * node, const std::string& name, const std::string & parentName) {
	assert(node != nullptr);
	std::string nodeName = name;
	if (name == "") {
		nodeName = "Node[" + std::to_string(++_counter) + "]";
		node->setName(nodeName);
	}

#ifdef CONSOLE_LOG
	std::cout << "<Scene::addChild>Adding a Scene Node with the name " << nodeName << " and the parent " << parentName << std::endl;
#endif

	/* Search for the parent: */
	SceneNode* parent = findByName(parentName);
	if (parent == nullptr) {
		throw initError(("<Scene::addChild> Could not find a parent with the name " + parentName).c_str());
	}

	/* link the child to its parent: */
	parent->addChild(node);

	/* put the child in the scene map: */
	_scene.insert(std::make_pair(nodeName, node));

	/*	Now we linked the node and its children to the node-net, we need to put it 
		and all children in the appropriate scene lists: */
	addToList(node);
}


void Scene::addToList(SceneNode* node) {
	assert(node != nullptr);

	/* put the child in the appropriate vector:*/
	if (node->obj() != nullptr) {
		IGameObject* object = node->obj();
		std::string nodeName = node->name();

		switch (node->type()) {
		case ObjType::shape:
		{
			Shape* s = static_cast<Shape*>(object);
			if (s->isTransparent()) {
				_transparent.insert(std::make_pair(nodeName, object));
			}
			else
				_shapes.insert(std::make_pair(nodeName, object));
		}
		break;
		case ObjType::mesh: 
		{
			Mesh* s = static_cast<Mesh*>(object);
			if (s->isTransparent()) {
				_transparent.insert(std::make_pair(nodeName, s));
			}
			else {
				_shapes.insert(std::make_pair(nodeName, s));
			}
		}
		break;
		case ObjType::dlight:
			_dlights.insert(std::make_pair(nodeName, static_cast<dLight*>(object)));
			break;
		case ObjType::plight:
		{
			pLight* light = static_cast<pLight*>(object);
			node->transform.setPos(light->pos());
			_plights.insert(std::make_pair(nodeName, light));
		}
		break;
		case ObjType::skybox:
			//TO DO: need to check for an existing skybox!
			_skybox = static_cast<Skybox*>(object);
			break;
		default:
			throw vitiError("<Scene::addChild>Unknown Object type.");
		}

		/* make sure the node is in the scene list */
		if (_scene[node->name()] == nullptr) {
			_scene.erase(node->name()); //this line should NOT be necessary, but it is atm -> see scene.print(), we have nullptrs
			_scene.insert(std::make_pair(node->name(), node));
		}
	}
	/* recursivly visit all children: */
	for (auto i = node->childrenBegin(); i < node->childrenEnd(); i++) {
		addToList(*i);
	}
}

/* takes a given scene node out of its respective list: */
void Scene::removeFromList(SceneNode * node) {
	assert(node);

	if (node->obj()) {
		std::string name = node->name();
		IGameObject* obj = node->obj();

		switch (obj->type()) {
		case ObjType::shape:
		{
			Shape* s = static_cast<Shape*>(obj);
			if (s->isTransparent()) _transparent.erase(name);
			else _shapes.erase(name);
		}
		break;
		case ObjType::mesh:
		{
			Mesh* m = static_cast<Mesh*>(obj);
			if (m->isTransparent()) _transparent.erase(name);
			else _shapes.erase(name);
		}
		break;
		case ObjType::dlight:
			if (_dshadowcaster == obj) _dshadowcaster = nullptr;
			_dlights.erase(name);
			break;
		case ObjType::plight:
			if (_pshadowcaster == obj) _pshadowcaster = nullptr;
			_plights.erase(name);
			break;
		case ObjType::skybox:
			_skybox = nullptr;
			break;
		}
	}

	/* also, take object out of the scene list: */
	_scene.erase(node->name());

	/* recursivly visit all children: */
	for (auto i = node->childrenBegin(); i < node->childrenEnd(); i++) {
		removeFromList(*i);
	}
}

void Scene::remove(const std::string & name) {
	SceneNode* node = findByName(name);
	if (!node) 
		throw vitiError(("<Scene::remove>Trying to remove an inexisting scene node with the name " + name).c_str());

	/* tell the nodes parent its child has died: */
	node->remove();

	/* take the nodes game object and all its children out of the respective scene lists: */
	removeFromList(node);

	/* delete the node and all its children from memory: */
	delete node;
	node = nullptr;
}


SceneNode * Scene::findByName(const std::string & name) {
	auto node = _scene.find(name);
	if (node == _scene.end()) throw vitiError(("<Scene::findByName> Could not find scene node named " + name).c_str());
	return node->second;
}


void Scene::update(const Uint32 & deltaTime) {
	_root->update(deltaTime);
}

void Scene::drawShapes(const Shader & shader) {
	for (const auto& S : _shapes) S.second->draw(shader);
}

void Scene::drawShapes(const Shader & shader, vitiGEO::Frustum& frustum) {
	_cullingList.clear();
	updateCullingList(frustum, _root);

	for (auto& N : _cullingList) N->draw(shader); 
}

void Scene::drawTransparent(const Shader & shader, vitiGEO::Frustum & frustum) {
	//to do: frustum culling
	
	/*	sort objects by distance to camera. We take the aabbs center as comparison point */
	std::map<float, IGameObject*> sorted;
	for (auto& O : _transparent) {
		Shape* s = static_cast<Shape*>(O.second);

		/* this code leads to a program crash on exiting the app */
		if (s) {
			s->getAABB()->transform(O.second->matrix());
			float distance = glm::length(s->getAABB()->center() - _cam->pos());
			sorted[distance] = O.second;
		}
	}

	/* now draw in reverse order: */
	for (std::map<float, IGameObject*>::reverse_iterator i = sorted.rbegin(); i != sorted.rend(); ++i) {
		i->second->draw(shader);
	}
}

void Scene::drawShapesNaked(const Shader & shader) const {
	for (const auto& S : _shapes) S.second->drawNaked(shader);
}

void Scene::drawShapesNaked(const Shader & shader, vitiGEO::Frustum& frustum) {
	_cullingList.clear();
	updateCullingList(frustum, _root);

	for (auto& N : _cullingList) N->drawNaked(shader);
}

void Scene::drawDlights(const Shader & shader) const {
	for (const auto& L : _dlights) {
		L.second->setUniforms(shader);
		L.second->draw(shader);
	}
}

void Scene::drawPlights(const Shader & shader) const {
	for (const auto& L : _plights) L.second->draw(shader);
}

void Scene::drawSkybox(const Shader & shader) const {
	if (_skybox) _skybox->draw(shader);
}

void Scene::setLightningUniforms(const Shader & shader) const {
	for (const auto& L : _dlights) L.second->setUniforms(shader);
	for (const auto& L : _plights) L.second->setUniforms(shader);
}

void Scene::setShadowcaster(const std::string& name) {
	SceneNode* node = findByName(name);
	if (node == nullptr) throw vitiError(("<Scene::setShadowcatser>Trying to add an inexisting pLight named" + name).c_str());

	switch (node->type()) {
	case ObjType::plight:
		_pshadowcaster = static_cast<pLight*>(node->obj());
		break;
	case ObjType::dlight:
		_dshadowcaster = static_cast<dLight*>(node->obj());
		break;
	default:
		throw vitiError(("<Scene::setShadowcatser>Invalid type. Object with name " + name + " can not be a shadowcatser").c_str());
	}
}

void Scene::drawPShadows(const CamInfo & cam) {
	_pShadow.on();
	_pShadow.draw(_pshadowcaster, this, cam);
	_pShadow.off();
}

void Scene::drawDShadows(const CamInfo & cam, vitiGEO::Frustum& frustum) {
	_dShadow.on();
	_dShadow.draw(_dshadowcaster, this, cam, frustum);
	_dShadow.off();
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

void Scene::updateCullingList(vitiGEO::Frustum & frustum, SceneNode* from) {
	for (auto& S : _shapes) {
		glm::vec3 pos = _scene[S.first]->transform.pos();
		float radius = _scene[S.first]->radius();
		if (frustum.isInside(pos, radius)) _cullingList.push_back(_scene[S.first]);
	}
}
}
