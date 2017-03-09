#include "SceneSaver.hpp"

#include "Scene.hpp"
#include <iostream>

using namespace tinyxml2;

namespace vitiGL {

SceneSaver::SceneSaver(Scene* scene, const std::string& fileName) {
	if (scene) saveScene(scene, fileName);	
}

SceneSaver::~SceneSaver() {}

bool SceneSaver::saveScene(Scene* scene, const std::string& filename) {
	
	/* prepare the xml-doc*/
	XMLNode* xScene = _doc.NewElement("Scene");
	_doc.InsertFirstChild(xScene);

	//get the scenes root:
	SceneNode* sceneNode = scene->findByName("root");
	if (!sceneNode) return false;

	//visit the root node (and all children by recursion):
	processNode(sceneNode, xScene);

	XMLError result = _doc.SaveFile(filename.c_str());
	if (result != XML_SUCCESS) {
		std::cout << "<SceneSaver::SceneSaver>Could not save scene into file " << filename << std::endl;
	return false;
	}
	return true;
}


bool SceneSaver::processNode(SceneNode* sceneNode, XMLNode* parent) {
	//insert a new node:	
 	XMLNode* xnode = _doc.NewElement("Node");
	parent->InsertEndChild(xnode);

	//Node Name:
	XMLElement* nodeName = _doc.NewElement("Name");
	nodeName->SetText(sceneNode->name().c_str());
	xnode->InsertEndChild(nodeName);	
		
	//process the object depending on its type:
	if (sceneNode->obj()) processObj(sceneNode, xnode);
 	//process the objects transformation:
	processTransform(sceneNode, xnode);	
	//process physics properties, if any:
	if (sceneNode->physics()) processPhysics(sceneNode, xnode);

	//visit all the nodes children:
	auto begin = sceneNode->childrenBegin();
	auto end = sceneNode->childrenEnd();

	for (auto i = begin; i != end; i++) {
		processNode((*i), xnode);
	}

	return true;
}

bool SceneSaver::processObj(SceneNode* sceneNode, XMLNode* xmlNode) {
	XMLElement* tag = _doc.NewElement("Type");	

	switch (sceneNode->obj()->type()) {
	case ObjType::shape:
		tag->SetText("Shape");
		break;
	case ObjType::mesh:
		tag->SetText("Mesh");
		break;
	case ObjType::dlight:
		tag->SetText("dLight");
		break;
	case ObjType::plight:
		tag->SetText("pLight");
		break;
	case ObjType::skybox:
		tag->SetText("Skybox");
		break;
	}
	
	xmlNode->InsertEndChild(tag);
	return true;
}

bool SceneSaver::processTransform(SceneNode* sceneNode, XMLNode* parent) {
	//Make the transform tag:
	XMLNode* tag = _doc.NewElement("Transform");
	parent->InsertEndChild(tag);
	
	//store position:
	XMLNode* posNode = _doc.NewElement("Pos");
	tag->InsertEndChild(posNode);

	glm::vec3 pos = sceneNode->transform.pos();

	insert(posNode, "X", pos.x);
	insert(posNode, "Y", pos.y);
	insert(posNode, "Z", pos.z);

	//store rotation (as quaternion):
	XMLNode* rotNode = _doc.NewElement("Rotation");
	tag->InsertEndChild(rotNode);

	glm::quat o = sceneNode->transform.orientation();

	insert(rotNode, "W", o.w);
	insert(rotNode, "X", o.x);
	insert(rotNode, "Y", o.y);
	insert(rotNode, "Z", o.z);

	//store the scaling:
	XMLNode* scaleNode = _doc.NewElement("Scale");
	tag->InsertEndChild(scaleNode);

	glm::vec3 scale = sceneNode->transform.scale();

	insert(scaleNode, "X", scale.x);
	insert(scaleNode, "Y", scale.y);
	insert(scaleNode, "Z", scale.z);

	return true;
}

bool SceneSaver::processPhysics(SceneNode* sceneNode, XMLNode* parent) {
	if (!sceneNode->physics()) return false;

	XMLNode* physics = _doc.NewElement("Physics");
	parent->InsertEndChild(physics);

	std::string typeName;
	/* switch depending on the physics body: 
	   (maybe put this switch into PhysicObject.hpp as function "typeName") */
	vitiGEO::BodyType type = sceneNode->physics()->type();
	switch (type) {
	case vitiGEO::BodyType::cone:
		typeName = "cone";
		break;
	case vitiGEO::BodyType::convexHull:
		typeName = "convexHull";
		break;
	case vitiGEO::BodyType::cuboid:
		typeName = "cuboid";
		break;
	case vitiGEO::BodyType::cylinder:
		typeName = "cuboid";
		break;
	/* This one will be difficult to implement... */
	case vitiGEO::BodyType::mulitBody:
		typeName = "multiBody";
		break;
	case vitiGEO::BodyType::plane:
		typeName = "plane";
		break;
	case vitiGEO::BodyType::sphere:
		typeName = "sphere";
		break;
	default:
		throw vitiError("<SceneSaver::processPhysics: Physic Object is of an unknown type.");
	}

	insert(physics, "Type", typeName);

	/* we also store the mass and the friction values. We do NOT store velocity etc. atm */
	vitiGEO::PhysicObject* obj = sceneNode->physics();

	float mass = obj->mass();
	insert(physics, "Mass", mass);
	float friction = obj->friction();
	insert(physics, "Friction", friction);
	float rollingFriction = obj->rollingFriction();
	insert(physics, "RollingFriction", rollingFriction);
	float spinningFriction = obj->getSpinningFriction();
	insert(physics, "SpinningFriction", spinningFriction);
	float damping = obj->damping();
	insert(physics, "Damping", damping);
	float angularDamping = obj->angularDamping();
	insert(physics, "AngularDamping", angularDamping);
}

void SceneSaver::insert(tinyxml2::XMLNode * parent, const std::string & tagName, const std::string & value) {
	XMLElement* tag = _doc.NewElement(tagName.c_str());
	tag->SetText(value.c_str());
	parent->InsertEndChild(tag);
}

void SceneSaver::insert(tinyxml2::XMLNode * parent, const std::string & tag, float value) {
	insert(parent, tag, std::to_string(value));
}

}
