/*	CLASS SCENE_NODE -------------------------------------------------------------------------
	
	Tasks:	
	1)	Hold the objects in our scene and establish a parent-child relationship between
		them. Also, together with class Scene, perform viewFrustum culling

	2)	SceneNode is the main interface for the user to interact with the game objects. I

	3)  Also, the sceneNode class is responsible for deleting all game objects. !!
------------------------------------------------------------------------------------------------ */


#pragma once

#include <SDL2/SDL.h>
#include <glm/gtc/matrix_transform.hpp>
#include <map>

#include <Frustum.hpp>
#include <PhysicObject.hpp>
#include <Transform.hpp>

#include "Shape.hpp"
#include "Error.hpp"
#include "Light.hpp"
#include "Shadowmap.hpp"


namespace vitiGL {

class Frustum;
class Skybox;
class Camera;

/*	CLASS SceneNode ------------------------------------------------------------------------------ */
class SceneNode {
public:
	SceneNode(const std::string& name, 
			  IGameObject* object = nullptr, 
			  glm::vec3 pos = { 0.0f, 0.0f, 0.0f });

	virtual ~SceneNode();
	
	virtual void update(const Uint32& deltaTime);

	virtual void draw(const Shader& shader) const;
	virtual void drawNaked(const Shader& shader) const;
	virtual void drawChilds(const Shader& shader) const;

	virtual void addChild(SceneNode* s);
	virtual void removeChild(SceneNode* s); //should make this a private function?

	virtual void remove();

	/*	add physics component to this element:
	@f : the mass for cuboids, the distance for planes
	@v : the velocity for cuboids, the normal for planes
	*/
	void addPhysics(vitiGEO::BodyType type, float f, const glm::vec3& v = { 0.0f, 0.0f, 0.0f });
	void removePhysics();

	/*	merge several objects into a physics compund shape 
		! we take the btCubeShape for every part right now ! */
	void addCompoundPhysics(const std::vector<SceneNode*>& objects, 
						   const std::vector<float>& masses,
						   const glm::vec3& pos,
						   const glm::vec3& velocity = {});

	/*	getters and setters: */
	float		radius() const { return _radius; }

	void setName(const std::string& name) { _name = name; }
	std::string name() { return _name; }

	ObjType		type() { return _obj->type(); }

	IGameObject* obj() { return _obj; }
	vitiGEO::PhysicObject* physics() { return _physics; }

	/* get an iterator to the childrens vector: */
	SceneNode*	parent()		{ return _parent; }
	auto		childrenBegin() { return _children.begin(); }
	auto		childrenEnd()	{ return _children.end(); }

	/* override transform: */
	void setTransform(const vitiGEO::Transform& t) { transform = t; }

	/* public variable transform for accessing translations, rotations etc.: */
	vitiGEO::Transform transform;

protected:
	/* private constructor, should be used only by classes inheriting from SceneNode: */
	SceneNode() {}

	IGameObject* _obj;
	vitiGEO::PhysicObject* _physics;

	SceneNode*	_parent;
	std::vector<SceneNode*> _children;

	float		_radius;

	std::string _name;
};

/*	CLASS SCENE ------------------------------------------------------------------------------
	Task:	Wrapper to identify Scene Nodes by name and adding childs to a parent by the
			parent name.

			KEEP TRACK OF ALL AND EVERY GAME OBJECT AND MANAGE THEIR DESTRUCTION:
			- functions addToList and removeFromList are at the core of this
--------------------------------------------------------------------------------------------- */

class Scene {
public:
	Scene();
	~Scene();

	void addChild	(IGameObject* object, 
					glm::vec3 pos = {},
					const std::string& name = "", 
					const std::string& parentName = "root");

	void addChild	(IGameObject* object,
					const std::string& name = "",
					const std::string& parentName = "root");

	void addChild	(SceneNode* node, 
					const std::string& name = "",
					const std::string& parentName = "root");

	/* removes node and all its children: */
	void remove(const std::string& name);

	void addCamera(Camera* cam) { _cam = cam; }

	/* access scene elements: */
	SceneNode* findByName(const std::string& name);
	
	//update all scene objects
	void update(const Uint32& deltaTime);

	//check lists for nullpointer sceneNodes and remove them:
	void updateLists();

	/* draw every Shape in the scene: */
	void drawShapes(const Shader& shader);
	/* draw every Shape in the scene with view-frustum culling: */
	void drawShapes(const Shader& shader, vitiGEO::Frustum& frustum);
	/* draw transparent Shapes: */
	void drawTransparent(const Shader& shader, vitiGEO::Frustum& frustum);
	/* draw everything, but without textures (->for the shadowmap) */
	void drawShapesNaked(const Shader& shader) const;
	void drawShapesNaked(const Shader& shader, vitiGEO::Frustum& frustum);

	/* draw lights (for second pass in deferred rendering) */
	void drawDlights(const Shader& shader) const;
	void drawPlights(const Shader& shader) const;

	/* draw Skybox (blit a depth buffer before using!): */
	void drawSkybox(const Shader& shader) const;

	/* set lightning uniforms (only needed for forward rendering): */
	void setLightningUniforms(const Shader& shader) const;

	/* draw shadowmaps: */
	void setShadowcaster(const std::string& name);
	void drawPShadows(const CamInfo& cam);
	void drawDShadows(const CamInfo& cam, vitiGEO::Frustum& frustum);

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

	//debug:
	void print() {
		std::cout << "--------------- SCENE NODES:\n";
		for (auto& S : _scene) {
			std::cout << "scene node name: " << S.first << "\t\tMemory address: " << S.second << std::endl;
		}

		std::cout << "--------------- SHAPE LIST:\n";
		for (auto& S : _shapes) {
			std::cout << S.first << "\t\tMemory address: " << S.second << std::endl;
		}

		std::cout << "--------------- TRANSPARENT LIST:\n";
		for (auto& S : _transparent) {
			std::cout << S.first << "\t\tMemory address: " << S.second << std::endl;
		}
	}

	//temporary function, needs another solution:
	dLight* findDLight(const std::string& name);
	pLight* findPLight(const std::string& name);

private:
	void updateCullingList(vitiGEO::Frustum& frustum, SceneNode* from);
	void addToList(SceneNode* node);
	void removeFromList(SceneNode* node);

	int _counter;

	SceneNode* _root; //we store root so we dont need to search it

	/* seperate lists for drawing: */
	std::map<std::string, SceneNode*> _scene;	//only used by SearchByName
	std::vector<SceneNode*>	_cullingList;
	std::map<std::string, IGameObject*>	_shapes;
	std::map<std::string, IGameObject*> _transparent;
	std::map<std::string, dLight*>	_dlights;
	std::map<std::string, pLight*>	_plights;

	Camera*			_cam;

	Skybox*			_skybox;

	pLight*			_pshadowcaster;
	dLight*			_dshadowcaster;
	PointShadowmap	_pShadow;
	DirShadowmap	_dShadow;
};

}
