#pragma once

#include <string>

#include "tinyxml2.h"

namespace vitiGL {

class Scene;
class SceneNode;

class SceneSaver {
public:
	SceneSaver(Scene* scene, const std::string& fileName);
	~SceneSaver();

private:
	bool saveScene(Scene* scene, const std::string& fileName);
	bool processNode(SceneNode* sceneNode, tinyxml2::XMLNode* parent);
	bool processObj(SceneNode* sceneNode, tinyxml2::XMLNode* parent);
	bool processTransform(SceneNode* sceneNode, tinyxml2::XMLNode* parent);
	bool processPhysics(SceneNode* sceneNode, tinyxml2::XMLNode* parent);

	void processDLight(SceneNode* sceneNode, tinyxml2::XMLNode* parent);
	void processPLight(SceneNode* sceneNode, tinyxml2::XMLNode* parent);
	void processSkybox(SceneNode* sceneNode, tinyxml2::XMLNode* parent);

	void processConfigFile(SceneNode* sceneNode, tinyxml2::XMLNode* parent);

	void processSubType(SceneNode* sceneNode, tinyxml2::XMLNode* parent);

	void insert(tinyxml2::XMLNode* parent, const std::string& tag, const std::string& value);
	void insert(tinyxml2::XMLNode* parent, const std::string& tag, float value);


	tinyxml2::XMLDocument _doc;
};
}
