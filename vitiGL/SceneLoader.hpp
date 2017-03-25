/* 
 * Class to load and create a scene from an xml-file generated with class SceneSaver 
 * 
 * To do: 
 *		-> IMPORTANT: Add support for sceneNodes sub-children
 * 		-> add support for multiBodies...
 */


#pragma once

#include "tinyxml2.h"

#include <string>
#include <Transform.hpp>

namespace vitiGL {

class Scene;
class SceneNode;
class Camera;

class SceneLoader {
public:
	SceneLoader(Scene* scene, Camera* cam, const std::string& fileName); //camera is needed for plight only...
	~SceneLoader();

private:
	bool processNode(SceneNode* sceneParent, tinyxml2::XMLNode* xmlParent); 
	
	void processShape(SceneNode* sceneNode, tinyxml2::XMLNode* xmlParent);
	void processMesh(SceneNode* sceneNode, tinyxml2::XMLNode* xmlParent);
	void processDLight(SceneNode* sceneNode, tinyxml2::XMLNode* xmlParent);
	void processPLight(SceneNode* sceneNode, tinyxml2::XMLNode* xmlNode);
	void processSkybox(SceneNode* sceneNode, tinyxml2::XMLNode* xmlNode);

	vitiGEO::Transform 	processTransform	(tinyxml2::XMLNode* xmlNode);
	void processPhysics(SceneNode* sceneNode, tinyxml2::XMLNode* xmlNode); //xmlNode -> <Physics>

	
	glm::vec3 			getVector	 		(tinyxml2::XMLNode* xmlNode, const std::string& name, bool RGB = false);
	std::string 		getString			(tinyxml2::XMLNode* xmlNode, const std::string& name);

	tinyxml2::XMLDocument	_doc;
	Scene* 					_scene;
	Camera* 				_cam;
	std::string 			_filename;
	
};
}
