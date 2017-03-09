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
		
	glm::vec3 pos = sceneNode->transform.pos();
	
	//make the position tag:
	XMLNode* posNode = _doc.NewElement("Pos");
	tag->InsertEndChild(posNode);

	//store position under X, Y and Z tags:
	XMLElement* xNode = _doc.NewElement("X");
	xNode->SetText(pos.x);
	posNode->InsertEndChild(xNode);
	XMLElement* yNode = _doc.NewElement("Y");
	yNode->SetText(pos.y);
	posNode->InsertEndChild(yNode);
	XMLElement* zNode = _doc.NewElement("Z");
	zNode->SetText(pos.z);
	posNode->InsertEndChild(zNode);

	//make the rotation tag:
	XMLNode* rotNode = _doc.NewElement("Rotation");
	tag->InsertEndChild(rotNode);

	glm::quat o = sceneNode->transform.orientation();
	//store quaternion under W, X, Y and Z tags:
	XMLElement* wNode = _doc.NewElement("W");
	wNode->SetText(o.w);
	rotNode->InsertEndChild(wNode);
	XMLElement* rxNode = _doc.NewElement("X");
	rxNode->SetText(o.x);
	rotNode->InsertEndChild(rxNode);
	XMLElement* ryNode = _doc.NewElement("Y");
	ryNode->SetText(o.y);
	rotNode->InsertEndChild(ryNode);
	XMLElement* rzNode = _doc.NewElement("Z");
	rzNode->SetText(o.z);
	rotNode->InsertEndChild(rzNode);

	//make the scaling tag:
	XMLNode* scaleNode = _doc.NewElement("Scale");
	tag->InsertEndChild(scaleNode);

	glm::vec3 scale = sceneNode->transform.scale();
	//store scale under X, Y and Z tags:
	XMLElement* sxNode = _doc.NewElement("X");
	sxNode->SetText(scale.x);
	scaleNode->InsertEndChild(sxNode);
	XMLElement* syNode = _doc.NewElement("Y");
	syNode->SetText(scale.y);
	scaleNode->InsertEndChild(syNode);
	XMLElement* szNode = _doc.NewElement("Z");
	szNode->SetText(scale.z);
	scaleNode->InsertEndChild(szNode);

	return true;
}

bool SceneSaver::processPhysics(SceneNode* sceneNode, XMLNode* parent) {
	if (!sceneNode->physics()) return false;

	
}

}
