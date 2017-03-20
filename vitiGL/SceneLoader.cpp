#include "SceneLoader.hpp"

#include "Scene.hpp"
#include "Model.hpp"
#include "Camera.hpp"
#include "Error.hpp"


using namespace tinyxml2;
using namespace vitiGEO;

namespace vitiGL {

SceneLoader::SceneLoader(Scene* scene, Camera* cam, const std::string& filePath) :
	_scene{ scene },
	_cam{ cam },
	_filename{ filePath }
{
	assert(_scene);
	assert(_cam);
	
	//open document and check for errors:	
	XMLError result = _doc.LoadFile(filePath.c_str());
	if (result != XML_SUCCESS) throw vitiError(("<SceneLoader::SceneLoader> Could not open file " + filePath).c_str());

	//check if we have an xml-root:
	XMLNode* xmlRoot = _doc.FirstChild();
	if (!xmlRoot) throw vitiError(("<SceneLoader::SceneLoader> Did not found a root node in file " + filePath).c_str());

	//Find the scene root:
	XMLNode* root = xmlRoot->FirstChildElement("Node");	
	if (!root) throw vitiError(("<SceneLoader::SceneLoader> Error while parsing file " + filePath).c_str());
	XMLElement* rootName = root->FirstChildElement("Name");
	std::string name = rootName->GetText();
	if (name != "root") throw vitiError(("<SceneLoader::SceneLoader> Did not find scene root in file " + filePath).c_str());

	//now process all the roots children by recursion:
	processNode(_scene->findByName("root"), root);	
}

SceneLoader::~SceneLoader() 
{}

bool SceneLoader::processNode(SceneNode* sceneParent, XMLNode* xmlParent) {
	std::string name = getString(xmlParent, "Name");
	std::cout << "<SceneLoader> Processing a scene node named " << name << std::endl;
	
	//get the nodes type and switch from there:
	XMLElement* element = xmlParent->FirstChildElement("Type");
	if (!element) {
		//make empty scene node
	}
	else {
		std::string type = element->GetText();
		if (type == "Mesh") processMesh(sceneParent, xmlParent);
		else if (type == "Shape") processShape(sceneParent, xmlParent);
		else if (type == "dLight") processDLight(sceneParent, xmlParent);
		else if (type == "pLight") processPLight(sceneParent, xmlParent);
		else if (type == "Skybox");	
	}
	
	//process all children of this node:
	int i = 0;
	for (XMLNode* child = xmlParent->FirstChildElement("Node"); child != nullptr; child = child->NextSiblingElement("Node")) {
		std::cout << "Iteration no " << i << std::endl;
		i++;
		processNode(sceneParent, child);
	}	

	return true;
}


void SceneLoader::processShape(SceneNode* sceneParent, XMLNode* xmlParent) {

	//get the Transform node:
	XMLNode* tNode = xmlParent->FirstChildElement("Transform");
	if (!tNode) throw vitiError(("<SceneLoader::processShape> Could not find a transform tag in file " + _filename).c_str());

	Transform t			{ processTransform(tNode) };
	std::string name	{ getString(xmlParent, "Name") };
	std::string parentName{ getString(xmlParent->Parent(), "Name") };
	std::string configFile{ getString(xmlParent, "ResourceFile") };

	//we need the Shapes Subtype:
	ShapeType subtype = stringToShapeType( getString(xmlParent, "Subtype"));

	//add to scene:
	switch (subtype) {
		case ShapeType::sphere:
			_scene->addChild(new Sphere{ configFile }, glm::vec3{}, name, parentName);
			break;
		case ShapeType::cuboid:
			_scene->addChild(new Cuboid{ configFile }, name, parentName);
			break;
		case ShapeType::icosahedron:
			_scene->addChild(new Icosahedron{ configFile }, name, parentName);
			break;
		case ShapeType::octahedron:
			_scene->addChild(new Octahedron{ configFile }, name, parentName);
			break;
		case ShapeType::tetrahedron:
			_scene->addChild(new Tetrahedron{ configFile }, name, parentName);
			break;
	}
	_scene->findByName(name)->setTransform(t);

	//check if physics is attached:
	XMLNode* pNode = xmlParent->FirstChildElement("Physics");
	if (pNode) processPhysics(_scene->findByName(name), pNode);
}


void SceneLoader::processMesh(SceneNode* sceneNode, XMLNode* xmlNode) {
	XMLNode* tNode = xmlNode->FirstChildElement("Transform");
	if (!tNode) throw vitiError(("<SceneLoader::processMesh> Could not find a transform tag in file " + _filename).c_str());
	
	Transform t			{ processTransform(tNode) };
	std::string name	{ getString(xmlNode, "Name") };
	std::string parentName{ getString(xmlNode->Parent(), "Name") };
	std::string configFile{ getString(xmlNode, "ResourceFile") };

	_scene->addChild(new Model{ configFile }, name, parentName);
	_scene->findByName(name)->setTransform(t);


	//check for physics:
	XMLNode* pNode = xmlNode->FirstChildElement("Physics");
	if (pNode) processPhysics(_scene->findByName(name), pNode);	
}


void SceneLoader::processDLight(SceneNode* sceneNode, XMLNode* xmlNode) {
	glm::vec3 dir 		{ getVector(xmlNode, "Direction") };
	glm::vec3 diffuse	{ getVector(xmlNode, "Diffuse", true) };
	glm::vec3 specular 	{ getVector(xmlNode, "Specular", true) };
	std::string name 	{ getString(xmlNode, "Name") };
	std::string parentName{ getString(xmlNode->Parent(), "Name") }; 				

	dLight* light = new dLight{ name, dir };
	light->setProperty(lightProps::diffuse, diffuse);
	light->setProperty(lightProps::specular, specular);
	_scene->addChild(light, name, parentName);

	//to do: store shadowcaster in xml
	_scene->setShadowcaster(name);
}


void SceneLoader::processPLight(SceneNode* sceneNode, XMLNode* xmlNode) {
	glm::vec3 pos 		{ getVector(xmlNode, "Position") };
	glm::vec3 diffuse 	{ getVector(xmlNode, "Diffuse", true) };
	glm::vec3 specular	{ getVector(xmlNode, "Specular", true) };
	glm::vec3 attenuation{ getVector(xmlNode, "Attenuation") };
	std::string name 	{ getString(xmlNode, "Name") };
	std::string parentName{ getString(xmlNode->Parent(), "Name") };

	pLight* light = new pLight{ _cam };
  	light->setProperty(lightProps::pos, pos);
	light->setProperty(lightProps::diffuse, diffuse);
	light->setProperty(lightProps::specular, specular);
	light->setProperty(lightProps::attenuation, attenuation);

	_scene->addChild(light, name, parentName);
	_scene->setShadowcaster(name);
	
}

Transform SceneLoader::processTransform(XMLNode* xmlParent) {
	Transform t;
	
	float x{ 0.0f };
	float y{ 0.0f };
	float z{ 0.0f };
	float w{ 0.0f };
	
	//get Position:
	XMLNode* posNode = xmlParent->FirstChildElement("Pos");
	if (!posNode) throw vitiError(("<SceneLoader::processTransform> 'Pos'-Tag missing in file " + _filename).c_str());
			
	XMLElement* posXNode = posNode->FirstChildElement("X");
	posXNode->QueryFloatText(&x);
	XMLElement* posYNode = posNode->FirstChildElement("Y");
	posYNode->QueryFloatText(&y);
	XMLElement* posZNode = posNode->FirstChildElement("Z");
	posZNode->QueryFloatText(&z);

	t.setPos(glm::vec3{ x, y, z});

	//get Scale:
	XMLNode* scaleNode = xmlParent->FirstChildElement("Scale");
	if (!scaleNode) throw vitiError(("<SceneLoader::processTransform> 'Scale'-Tag missing in file " + _filename).c_str());

	XMLElement* scaleXNode = scaleNode->FirstChildElement("X");
	scaleXNode->QueryFloatText(&x);
	XMLElement* scaleYNode = scaleNode->FirstChildElement("Y");
	scaleYNode->QueryFloatText(&y);
	XMLElement* scaleZNode = scaleNode->FirstChildElement("Z");
	scaleZNode->QueryFloatText(&z);

	t.setScale(glm::vec3{ x, y, z });

	//get Rotation:
	XMLNode* rotNode = xmlParent->FirstChildElement("Rotation");
	if (!rotNode) throw vitiError(("<SceneLoader::processTransform> 'Rotation'-Tag missing in file " + _filename).c_str());

	XMLElement* rotWNode = rotNode->FirstChildElement("W");
	rotWNode->QueryFloatText(&w);
	XMLElement* rotXNode = rotNode->FirstChildElement("X");
	rotXNode->QueryFloatText(&x);
	XMLElement* rotYNode = rotNode->FirstChildElement("Y");
	rotYNode->QueryFloatText(&y);
	XMLElement* rotZNode = rotNode->FirstChildElement("Z");
	rotZNode->QueryFloatText(&z);

	t.rotateTo(glm::quat{ w, x, y, z });

	return t;
}	


void SceneLoader::processPhysics(SceneNode* sceneNode, XMLNode* xmlNode) {
		std::cout << "<SCENE LOADER> Processing Physics of a scene node named " << sceneNode->name() << std::endl;
		
	float mass				{ 0.f };
	float friction			{ 0.f };
	float rollingFriction	{ 0.f };
	float spinningFriction	{ 0.f };
	float damping 			{ 0.f };
	float angularDamping 	{ 0.f };

	(xmlNode->FirstChildElement("Mass"))->QueryFloatText(&mass);
	(xmlNode->FirstChildElement("Friction"))->QueryFloatText(&friction);
	(xmlNode->FirstChildElement("RollingFriction"))->QueryFloatText(&rollingFriction);
	(xmlNode->FirstChildElement("SpinningFriction"))->QueryFloatText(&spinningFriction);
	
	//add physics to this node:
	BodyType type = stringToBodyType(xmlNode->FirstChildElement("Type")->GetText());

	if (type != BodyType::mulitBody) {
		std::cout << "<SCENE LOADER> Adding physics. Type: " << xmlNode->FirstChildElement("Type")->GetText() << " mass: " << mass << "\n";
		sceneNode->addPhysics(type, mass);
		PhysicObject* obj = sceneNode->physics();
		obj->setFriction(friction);
		obj->setRollingFriction(rollingFriction);
		obj->setSpinningFriction(spinningFriction);
		obj->setDamping(damping);
		obj->setAngularDamping(angularDamping);
	}
	else std::cout << "<SCENE LOADER> No physics added!\n";
	//TO DO: ADD SUPPORT FOR MULTI-BODIES HERE .....
}

glm::vec3 SceneLoader::getVector(XMLNode* xmlNode, const std::string& name, bool RGB /* = false */) {
	XMLElement* elem = xmlNode->FirstChildElement(name.c_str());
	if (!elem) throw vitiError(("<SceneLoader::getVector> Error while parsing file " + _filename).c_str());

	float x{ 0.f };
	float y{ 0.f };
	float z{ 0.f };

	const char* one = RGB? "R" : "X";	
	const char* two = RGB? "G" : "Y";
	const char* three = RGB? "B" : "Z";

	elem->FirstChildElement(one)->QueryFloatText(&x);
	elem->FirstChildElement(two)->QueryFloatText(&y);
	elem->FirstChildElement(three)->QueryFloatText(&z);
	
	return glm::vec3{ x, y, z };
}

std::string SceneLoader::getString(XMLNode* xmlNode, const std::string& name) {
	
	XMLElement* elem = xmlNode->FirstChildElement(name.c_str());
	if (!elem) throw vitiError(("<SceneLoader::getString> Error while parsing file " + _filename + 
					"\nDid not find tag " + name).c_str());

	return elem->GetText();
}

}
