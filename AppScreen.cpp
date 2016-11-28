#include "AppScreen.hpp"
#include "vitiGL.hpp"

#include "App.hpp"

#include <conio.h>

#include "vitiGEO/Ray.hpp"
#include "vitiGEO/AABB.hpp"
#include "vitiGL/AABBShape.hpp"
#include "vitiGEO/Constraint.hpp"
#include "vitiGEO/DebugInfo.hpp"
#include "vitiGL/RayTriangle.hpp"

using namespace vitiGL;
using namespace vitiGEO;

AppScreen::AppScreen(App* app, vitiGL::Window* window)
	: IAppScreen{ app },
	_cam{ float(window->width()) / float(window->height()) },
	_renderer{ window, &_scene, &_cam },
	_drender{ window, &_scene, &_cam },
	_gui{ "GUI", "TaharezLook.scheme" },
	_console{ this, &_gui, "layouts/console.layout"},

	/* TEST: Work with bullet physics */
	_btConfig{ new btDefaultCollisionConfiguration() },
	_btDispatcher{ new btCollisionDispatcher(_btConfig) },
	_btBroadphase{ new btDbvtBroadphase() },
	_btSolver{ new btSequentialImpulseConstraintSolver() },
	_btWorld{ new btDiscreteDynamicsWorld(_btDispatcher, _btBroadphase, _btSolver, _btConfig) }

{
	_btWorld->setGravity(btVector3{ 0.0f, -9.8f, 0.0f });

	_index = SCREEN_INDEX_APP;
	RayTriangle::start();

	/* Create the scene elements: */
	int prefix = 1;
	for (int i = -3; i < 3; i++) {
		prefix = (prefix == 1) ? -1 : 1;
		for (int j = 1; j < 7; j++) {
			std::string parentName = "Cube" + std::to_string(i) + "/" + std::to_string(j);
			//std::string childName = "Cuboid" + std::to_string(i) + "/" + std::to_string(j);
			//std::string child2Name = "SmallCuboid" + std::to_string(i) + "/" + std::to_string(j);

			_scene.addChild(new Cuboid{ "xml/cube.xml" }, glm::vec3{ prefix * i * 4.0, j * 4.0f, i * 4.0 }, parentName);
			
			
			//_scene[parentName]->addPhysics(40.0f);
			//_scene[parentName]->transform.scale(glm::vec3{ 2.0f, 2.0f, 2.0f });
			//_scene.addChild(new Cuboid{ "xml/cubeSmall.xml" }, glm::vec3{ 3.0, 0.0f, 3.0 }, childName, parentName);
			//_scene[childName]->addPhysics(5.0f);
			//_scene.addChild(new Cuboid{ "xml/cubeSmall.xml" }, glm::vec3{ 0.0, prefix * 3.0f, 0.0 }, child2Name, childName);
			//_scene[child2Name]->addPhysics(5.0f);
			//_scene[child2Name]->transform.scale(glm::vec3{ 0.6f, 0.6f, 0.6f });
		}
	}
	_scene.addChild(new Cuboid{ "xml/cube.xml" }, glm::vec3{ 0.0f, 2.0f, -1.0f }, "Cuboid");
	btTransform ct;
	ct.setIdentity();
	ct.setOrigin(btVector3{ 0.0f, 2.0f, -1.0f });
	btBoxShape* cShape = new btBoxShape(btVector3{ 0.5f, 0.5f, 0.5f });
	btMotionState* cState = new btDefaultMotionState(ct);
	btVector3 inertia;
	cShape->calculateLocalInertia(10.0f, inertia);
	btRigidBody::btRigidBodyConstructionInfo cInfo = { btScalar(10.0f), cState, cShape, inertia };
	btRigidBody* cBody = new btRigidBody{ cInfo };
	_btWorld->addRigidBody(cBody);
	_btBodies.insert(std::make_pair("Cuboid", cBody));

	//_scene["Cuboid"]->addPhysics(10.0f);

	/*_scene.addChild(new Model{ "Models/Old House/Old House 2 3D Models.obj", &_cam, false }, "Shark");
	_scene["Shark"]->transform.scale(glm::vec3{ 0.05f, 0.05f, 0.05f });
	_scene["Shark"]->transform.setPos(glm::vec3{ -5.0f, 0.0f, -5.0f });*/
	//_scene["Shark"]->addPhysics(10.0f);
	//_scene["Shark"]->removePhysics();


	_scene.addChild(new Cuboid{ "xml/cube_floor.xml" }, glm::vec3{ -3.0f, -3.0f, -3.0f }, "Floor");
	//_scene["Floor"]->addPhysics(10000.0f);
	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3{ -0.0f, -3.0f, -0.0f });
	btStaticPlaneShape* floor = new btStaticPlaneShape(btVector3{ 0.0f, 1.0f, 0.0f }, btScalar(3.0f));
	btMotionState* motion = new btDefaultMotionState(t);
	btRigidBody::btRigidBodyConstructionInfo info{ btScalar(0.0f), motion, floor };
	btRigidBody* floorBody = new btRigidBody(info);
	_btWorld->addRigidBody(floorBody);
	_btBodies.insert(std::make_pair("Floor", floorBody));
	//_scene["Floor"]->physics()->setG(false);

	_scene.addChild(new Cuboid{ "xml/cube_floor.xml" }, glm::vec3{ 20.0f, 7.0f, -3.0f }, "Wall");
	//_scene["Wall"]->addPhysics(10000.0f);
	//_scene["Wall"]->physics()->setG(true);
	SceneNode* wall = _scene.findByName("Wall");
	wall->transform.rotate(90.0f, glm::vec3{ 0.0f, 0.0f, 1.0f });

	_scene.addChild(new dLight{ "dlight", glm::vec3{ 0.5f, -1.0f, -0.5f } }, "dlight");
	_scene.setShadowcaster("dlight");

	pLight* plight = new pLight{ &_cam };
	plight->setProperty(lightProps::pos, glm::vec3{ 0.0f, 20.0f, 0.0f });
	plight->setProperty(lightProps::diffuse, glm::vec3{ 10.0f, 5.0f, 0.0f });
	plight->setProperty(lightProps::specular, glm::vec3{ 20.0f, 10.0f, 0.0f });

	_scene.addChild(plight, "plight");
	_scene.setShadowcaster("plight");

	/* Skybox: */
	std::vector<std::string> faces;
	faces.push_back("Textures/Skybox/right.jpg");
	faces.push_back("Textures/Skybox/left.jpg");
	faces.push_back("Textures/Skybox/top.jpg");
	faces.push_back("Textures/Skybox/bottom.jpg");
	faces.push_back("Textures/Skybox/back.jpg");
	faces.push_back("Textures/Skybox/front.jpg");
	Skybox* skybox = new Skybox(faces);
	_scene.addChild(skybox, "Skybox");

	_scene.addCamera(&_cam);

	pLight* plight2 = new pLight{ &_cam };
	plight2->setProperty(lightProps::pos, glm::vec3{ 0.0f, 20.0f, 20.0f });
	plight2->setProperty(lightProps::diffuse, glm::vec3{ 10.0f, 5.0f, 0.0f });
	plight2->setProperty(lightProps::specular, glm::vec3{ 20.0f, 10.0f, 0.0f });

	_scene.addChild(plight2, "plight2");

	_cam.setPos(glm::vec3{ -4.0f, 8.0f, -5.0f });
	_cam.setTarget(glm::vec3{ 0.0f, 0.0f, 0.0f });

	initGUI();
	_timer.on();
}


AppScreen::~AppScreen() {
}

void AppScreen::onEntry() {
	SDL_ShowCursor(0);

	if (_timer.is_paused()) _timer.unpause();
}

void AppScreen::onExit() {
	globals::screenshot = _drender.texture();

	_timer.pause();
}

void AppScreen::update() {
	Uint32 frameTime = _timer.frame_time();
	Uint32 time = _timer.get_time();

	std::string fps = "FPS: " + std::to_string(_timer.fps());
	_console.setText(fps);
	_gui.update(frameTime);

	/*
	for (int i = -5; i < 4; i++) {
		for (int j = 1; j < 5; j++) {
			std::string parent = "Cube" + std::to_string(i) + "/" + std::to_string(j);
			std::string child = "Cuboid" + std::to_string(i) + "/" + std::to_string(j);
			std::string child2 = "SmallCuboid" + std::to_string(i) + "/" + std::to_string(j);

			auto temp = _scene[parent];
			temp->rotate(float(frameTime) / (20.0f * i + 5), glm::vec3{ 0.0f, 1.0f, 0.0f });

			temp = _scene[child];
			temp->rotate(float(frameTime) * j / (10.0f * i + 5), glm::vec3{ 0.0f, 0.0f, 1.0f });

			temp = _scene[child2];
			temp->rotate(float(frameTime) * j/ (5.0f * i  + 5), glm::vec3{ 0.0f, 1.0f, 0.0f });
		}
	}*/
	/*
	Model* temp = static_cast<Model*>(_scene["Shark"]);
	if (_rotate) 
		temp->transform.rotate(float(frameTime) / (20.0f), glm::vec3{ 0.0f, 1.0f, 0.0f });*/

	/* update all components: */
	updateInput();
	_btWorld->stepSimulation(float(frameTime) / 1000.0f);
	for (auto& body : _btBodies) {
		btTransform t;
		glm::mat4 M;
		float btM[16];

		body.second->getMotionState()->getWorldTransform(t);
		btVector3 pos = body.second->getCenterOfMassPosition();

		std::cout << "bt pos[" << body.first << "]: "  << pos.getX() << "/" << pos.getY() << "/" << pos.getZ() << std::endl;
		
		t.getOpenGLMatrix(btM);
		M[0][0] = btM[0]; M[0][1] = btM[1]; M[0][2] = btM[2]; M[0][3] = btM[3];
		M[1][0] = btM[4]; M[1][1] = btM[5]; M[1][2] = btM[6]; M[1][3] = btM[7];
		M[2][0] = btM[8]; M[2][1] = btM[9]; M[2][2] = btM[10]; M[2][3] = btM[11];
		M[3][0] = btM[12]; M[3][1] = btM[13]; M[3][2] = btM[14]; M[3][3] = btM[15];

		btQuaternion btO = t.getRotation();
		float angle = btO.getAngle();
		btVector3 axis = btO.getAxis();

		_scene[body.first]->transform.setPos(vitiGEO::getTranslation(M));
		_scene[body.first]->transform.rotate(angle, glm::vec3{ axis.getX(), axis.getY(), axis.getZ() });

		glm::vec3 glPos = _scene[body.first]->transform.pos();
		std::cout << "gl pos[" << body.first << "]: " << glPos.x << "/" << glPos.y << "/" << glPos.z << std::endl;

	}
	
	//PhysicEngine::instance()->update(frameTime);

	//DEBUG: Track the cube
	vitiGEO::DebugInfo::instance()->addLine(glm::vec4{ _cam.pos() + _cam.dir(), 0.05f }, glm::vec4{ _scene["Cuboid"]->transform.pos(), 0.05f });
	
	_scene.update(frameTime);
	_cam.update();
	_gui.update(frameTime);

}

void AppScreen::draw() {
	_drender.draw();
	PhysicEngine::instance()->drawDebug();
	_gui.draw();
}

int AppScreen::next() const {
	return SCREEN_INDEX_NONE;
}

int AppScreen::previous() const {
	return SCREEN_INDEX_MENU;
}

/* very simple command parser for console input: */
std::string AppScreen::command(const std::string & input) {
	
	/* we need a lot of string manipulation here... */
	std::string::size_type commandEnd = input.find(" ", 1);
	std::string command = input.substr(0, commandEnd);
	std::string argument = input.substr(commandEnd + 1, input.length() - commandEnd + 1);

	float value = atof(argument.c_str());

	std::cout << "Command: " << command << " /Value: " << argument << std::endl;

	/* convert command to lower-case: */
	for (size_t i = 0; i < command.size(); i++) {
		command[i] = tolower(command[i]);
	}

	/* process command: */
	if (command == "exit" || command == "quit") {
		_state = ScreenState::exit;
		return "Exiting app";
	}
	if (command == "setgamma") {
		_drender.setGamma(value);
		return "Setting gamma correction to " + std::to_string(value);
	}
	if (command == "getgamma") {
		return std::to_string(_drender.gamma());
	}
	if (command == "setexposure") {
		_drender.setExposure(value);
		return "Setting hdr exposure to " + std::to_string(value);
	}
	if (command == "getexposure") {
		return std::to_string(_drender.exposure());
	}
	if (command == "setbloom") {
		_drender.setBloomTreshold(value);
		return "Setting bloom treshold to " + std::to_string(value);
	}
	if (command == "getbloom") {
		return std::to_string(_drender.bloomTreshold());
	}
	if (command == "glerror") {
		return std::to_string(glGetError());
	}
	if (command == "setwireframe") {
		_drender.setDrawMode();
		return "Switching between wireframe and normal drawing...";
	}
	if (command == "drawnormals") {
		_drender.drawNormals(true);
		return "Drawing objects vertex normals.";
	}
	if (command == "hidenormals") {
		_drender.drawNormals(false);
		return "Hiding objects vertex normals.";
	}

	return "";
}

void AppScreen::initGUI() {
	//_gui.setScheme("TaharezLook.scheme");
	_gui.setFont("DejaVuSans-10");
	_gui.setMouseCursor("AlfiskoSkin/MouseArrow");
}

void AppScreen::updateInput() {
	SDL_Event input;
	while (SDL_PollEvent(&input)) {
		_gui.onSDLEvent(input);
		switch (input.type) {
		case SDL_QUIT:
			_state = ScreenState::exit;
			break;
		case SDL_KEYDOWN:
			switch (input.key.keysym.sym) {
			case SDLK_ESCAPE:
				_state = ScreenState::previous;
				break;
			case SDLK_w:
				_cam.move(Move::forward);
				break;
			case SDLK_s:
				_cam.move(Move::backward);
				break;
			case SDLK_a:
				_cam.move(Move::left);
				break;
			case SDLK_d:
				_cam.move(Move::right);
				break;
			
			/* DEBUG: Control the cube :*/
			case SDLK_KP_8:
				_btBodies["Cuboid"]->applyCentralImpulse(btVector3{ 0.0f, 0.0f, 100.0f });
				break;
			case SDLK_KP_2:
				_btBodies["Cuboid"]->applyCentralImpulse(btVector3{ 0.0f, 0.0f, -100.0f });
				break;
			case SDLK_KP_4:
				_btBodies["Cuboid"]->applyCentralImpulse(btVector3{ -100.0f, 0.0f, 0.0f });
				break;
			case SDLK_KP_6:
				_btBodies["Cuboid"]->applyCentralImpulse(btVector3{ 100.0f, 0.0f, 0.0f });
				break;
			case SDLK_KP_7:
				_btBodies["Cuboid"]->applyCentralImpulse(btVector3{ 0.0f, 100.0f, 0.0f });
				break;
			case SDLK_KP_1:
				_btBodies["Cuboid"]->applyCentralImpulse(btVector3{ 0.0f, -100.0f, 0.0f });
				break;
			case SDLK_KP_9:
				_btBodies["Cuboid"]->applyTorque(btVector3{ 0.0f, 10.0f, 0.0f });
				break;
			case SDLK_KP_3:
				_btBodies["Cuboid"]->applyTorque(btVector3{ 0.0f, -10.0f, 0.0f });
				break;
			case SDLK_KP_5:
				_scene["Cuboid"]->transform.setWorldMatrix(glm::mat4{});
				break;
			case SDLK_SPACE:
			{
				btRigidBody* temp = addCube(10.0f, _cam.pos());
				glm::vec3 dir = _cam.dir() * 10.0f;
				temp->setLinearVelocity(btVector3{ dir.x, dir.y, dir.z });
			}
				break;
			case SDLK_F1:
				_console.setVisible(_console.isVisible()? false : true);
				break;
			//debug:
			case SDLK_F2:
				_scene.switchCull();
				break;
			case SDLK_g:
				_drender.gramSchmidt();
				break;
			break;
				case SDLK_F3:
				{
					pLight* light = _scene.findPLight("plight");
					glm::vec3 pos = light->pos();
					light->setProperty(lightProps::pos, glm::vec3{ pos.x + 1.0f, pos.y + 1.0f, pos.z + 1.0f });
				}
				break;
				case SDLK_F4:
				{
					pLight* light = _scene.findPLight("plight");
					glm::vec3 pos = light->pos();
					light->setProperty(lightProps::pos, glm::vec3{ pos.x - 1.0f, pos.y - 1.0f, pos.z -1.0f });
				}
				break;
			case SDLK_F5:
				break;
			case SDLK_F6:
				break;
			case SDLK_F7:
				break;
			case SDLK_F9:
				break;
			case SDLK_KP_PLUS:
				break;
			case SDLK_KP_MINUS:
				break;
			case SDLK_KP_MULTIPLY:
				break;
			case SDLK_KP_DIVIDE:
				break;
			}
			break;
		case SDL_MOUSEMOTION:
			int dx, dy;
			SDL_GetMouseState(&dx, &dy);
			_cam.rotate(dx, dy);
			break;
		case SDL_MOUSEWHEEL:
			_cam.zoom(input.wheel.y);
			break;
		case SDL_MOUSEBUTTONDOWN:
		{
			/* TEMPORARY DEBUG CODE - TEST RAY-OBJECT COLLISION DETECTION 
			
				this is messy and just here to see if we get correct results */

			//_scene.addChild(new Cuboid{ "xml/cubeSmall.xml" }, _cam.pos() + 30.0f * _cam.dir(), sqrt(1.0f));

			/*
			vitiGEO::Ray ray{ _cam.pos(), _cam.dir() * 30.0f };

			Model* temp = static_cast<Model*>(_scene.findByName("Shark"));
			vitiGEO::AABB* aabb = temp->aabb().at(2);

			glm::vec3 intersection;
			float f{ 0.0f };

			if (aabb->rayIntersection(ray, intersection, f)) {*/
				//_scene.addChild(new Octahedron{ "xml/cube.xml" }, intersection, sqrt(2.0f));

				
				/* test for detailed collision with the mesh: 
				glm::vec3 tuv{};
				std::vector<glm::vec3> vertices = temp->vertices().at(2);
				for (int i = 0; i < vertices.size();) {
					std::vector<glm::vec3> triangle;
					for (int j = 0; j < 3; j++) {
						glm::vec4 vertex = { vertices[i].x, vertices[i].y, vertices[i].z, 1.0f };
						vertex = temp->posMatrix() * vertex;
						triangle.push_back(glm::vec3{ vertex.x, vertex.y, vertex.z });
						i++;
					}
					if (ray.rayTriangleIntersect(triangle, tuv)) {
						std::cout << "---------------------------- HIT THE MESH!\n";
						std::cout << "Ray delta factor: " << tuv.x << std::endl;
						std::cout << "UV: " << tuv.y << "/" << tuv.z << std::endl;

						glm::vec3 hitPoint = ray._origin + glm::normalize(ray._delta) * tuv.x;

						_scene.addChild(new Cuboid{ "xml/cubeSmall.xml" }, hitPoint, sqrt(1.0f));
					}
				}*/

				/* TEST: USE TRANSFORM FEEDBACK: 
				auto i = temp->childrenBegin() + 2;
				
				Mesh* mesh = static_cast<Mesh*>((*i)->obj());

				std::vector<glm::vec3> output;
				RayTriangle::update(mesh, &ray, output);
				std::cout << "GPU HIT DETECTION RESULTS ----------------------\n";
				for (int i = 0; i < output.size(); i++) {
					std::cout << "Factor t: " << output[i].x << std::endl;
					std::cout << "uv: " << output[i].y << "/" << output[i].z << std::endl;

					//test drawing: 
					glm::vec3 hitPoint = ray._origin + glm::normalize(ray._delta) * output[i].x;
					_scene.addChild(new Cuboid{ "xml/cubeSmall.xml" }, hitPoint);
				}
			}*/
			
			/* END OF DEBUG CODE */
			
		}
			break;
		}
	}
}


btRigidBody* AppScreen::addCube(float mass, const glm::vec3 pos) {
	static int i = 0;
	std::string name = "Cube" + std::to_string(i++);

	_scene.addChild(new Cuboid{ "xml/cube.xml" }, pos, name);

	btVector3 btPos{ pos.x, pos.y, pos.z };
	btTransform ct;
	ct.setIdentity();
	ct.setOrigin(btPos);
	btBoxShape* cShape = new btBoxShape(btVector3{ 0.5f, 0.5f, 0.5f });
	btMotionState* cState = new btDefaultMotionState(ct);
	btVector3 inertia;
	cShape->calculateLocalInertia(10.0f, inertia);
	btRigidBody::btRigidBodyConstructionInfo cInfo = { btScalar(10.0f), cState, cShape, inertia };
	btRigidBody* cBody = new btRigidBody{ cInfo };
	_btWorld->addRigidBody(cBody);
	_btBodies.insert(std::make_pair(name, cBody));

	return cBody;
}