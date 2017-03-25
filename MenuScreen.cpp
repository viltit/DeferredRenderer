#include "MenuScreen.hpp"

#include "App.hpp"
#include "AppScreen.hpp"

#include <SceneSaver.hpp>

#include <sstream>
#include <iomanip>
#include <Physics.hpp>

using namespace vitiGL;
using namespace vitiGEO;

MenuScreen::MenuScreen(App* app, AppScreen* appScreen, vitiGL::Window* window) 
	:	IAppScreen	{ app },
		_appScreen  { appScreen },
		_gui		{ "GUI", "AlfiskoSkin.scheme" },
		_window		{ window }
{
	assert(_appScreen);

	_index = SCREEN_INDEX_MENU;

	/* Initialize gui elements: */
	_gui.setScheme("AlfiskoSkin.scheme");
	_gui.setFont("DejaVuSans-10");
	_gui.setMouseCursor("AlfiskoSkin/MouseArrow");

	SDL_ShowCursor(0);

	_menu = _gui.createLayout("layouts/menu.layout");
	initGUI();
}


MenuScreen::~MenuScreen() {
}

void MenuScreen::onEntry() {
	//hide save and load masks:
	_menu->getChild("loadPopup")->hide();
	_menu->getChild("savePopup")->hide();

	//make sure all other menu parts are shown:
	_menu->getChild("General")->show();
	_menu->getChild("PLight")->show();
	_menu->getChild("DLight")->show();	

	/* get lights RGB values and other data: */
	dLight* light = _appScreen->_scene.findDLight("dlight");

	if (light) {
		float f = 255.f / 400.f;
		glm::vec3 color = light->diffuse();

		_values["dLightDiffuseR"]->setCurrentValue(color.r * f);
		_values["dLightDiffuseG"]->setCurrentValue(color.g * f);
		_values["dLightDiffuseB"]->setCurrentValue(color.b * f);

		color = light->specular();
		_values["dLightSpecularR"]->setCurrentValue(color.r * f);
		_values["dLightSpecularG"]->setCurrentValue(color.g * f);
		_values["dLightSpecularB"]->setCurrentValue(color.b * f);

		color = light->dir();
		f = 1.f / 60.f;
		_values["dLightVectorX"]->setCurrentValue(color.x * f + 0.5f);
		_values["dLightVectorY"]->setCurrentValue(color.y * f + 0.5f);
		_values["dLightVectorZ"]->setCurrentValue(color.z * f + 0.5f);
	}

	pLight* plight = _appScreen->_scene.findPLight("plight");
	if (plight) {
		glm::vec3 color = plight->diffuse();
		float f = 255.f / 400.f;

		_values["pLightDiffuseR"]->setCurrentValue(color.r * f);
		_values["pLightDiffuseG"]->setCurrentValue(color.g * f);
		_values["pLightDiffuseB"]->setCurrentValue(color.b * f);

		color = plight->specular();
		_values["pLightSpecularR"]->setCurrentValue(color.r * f);
		_values["pLightSpecularG"]->setCurrentValue(color.g * f);
		_values["pLightSpecularB"]->setCurrentValue(color.b * f);

		f = 1.f / 60.f;
		color = plight->pos();
		_values["pLightPosX"]->setCurrentValue(color.x * f + 0.5f);
		_values["pLightPosY"]->setCurrentValue(color.y * f + 0.5f);
		_values["pLightPosZ"]->setCurrentValue(color.z * f + 0.5f);
	}

	_sliders["gammaSlider"]->setCurrentValue(_appScreen->_drender.gamma() / 3.0f);
	_sliders["bloomSlider"]->setCurrentValue(_appScreen->_drender.bloomTreshold() / 3.0f);
	_sliders["hdrSlider"]->setCurrentValue(_appScreen->_drender.exposure() / 3.0f); 
}

void MenuScreen::onExit() {
}

void MenuScreen::update() {
	updateInput();
	_appScreen->updateFreezed();
	_gui.update(_timer.frame_time());
}

void MenuScreen::draw() {
	_appScreen->draw();
	_gui.draw();
}

int MenuScreen::next() const {
	return SCREEN_INDEX_APP;
}

int MenuScreen::previous() const {
	return SCREEN_INDEX_NONE;
}

void MenuScreen::initGUI() {
	//Subscribe close button events:
	auto frameGeneral = static_cast<CEGUI::FrameWindow*>(_menu->getChild("General"));
	frameGeneral->subscribeEvent(
		CEGUI::FrameWindow::EventCloseClicked, 
		CEGUI::Event::Subscriber(&MenuScreen::onMainClose, this));
	auto framePLight = static_cast<CEGUI::FrameWindow*>(_menu->getChild("PLight"));
	framePLight->subscribeEvent(
		CEGUI::FrameWindow::EventCloseClicked,
		CEGUI::Event::Subscriber(&MenuScreen::onPLightClose, this));
	auto frameDLight = static_cast<CEGUI::FrameWindow*>(_menu->getChild("DLight"));
	frameDLight->subscribeEvent(
		CEGUI::FrameWindow::EventCloseClicked,
		CEGUI::Event::Subscriber(&MenuScreen::onDLightClose, this));
	auto saveWidget = static_cast<CEGUI::FrameWindow*>(_menu->getChild("savePopup"));
	saveWidget->subscribeEvent(
		CEGUI::FrameWindow::EventCloseClicked,
		CEGUI::Event::Subscriber(&MenuScreen::onSaveClose, this));
	auto loadWidget = static_cast<CEGUI::FrameWindow*>(_menu->getChild("loadPopup"));
	loadWidget->subscribeEvent(
		CEGUI::FrameWindow::EventCloseClicked,
		CEGUI::Event::Subscriber(&MenuScreen::onLoadClose, this));

	//Exit and Continue Button:
	auto exitButton = static_cast<CEGUI::PushButton*>(_menu->getChild("General")->getChild("quitButton"));
	exitButton->subscribeEvent(
		CEGUI::PushButton::EventClicked,
		CEGUI::Event::Subscriber(&MenuScreen::onExitClicked, this));

	auto continueButton = static_cast<CEGUI::PushButton*>(_menu->getChild("General")->getChild("continueButton"));
	continueButton->subscribeEvent(
		CEGUI::PushButton::EventClicked,
		CEGUI::Event::Subscriber(&MenuScreen::onContinueClicked, this));

	//Save Scene and load Scene Buttons:
	auto loadButton = static_cast<CEGUI::PushButton*>(_menu->getChild("General")->getChild("loadButton"));
	loadButton->subscribeEvent(
		CEGUI::PushButton::EventClicked,
		CEGUI::Event::Subscriber(&MenuScreen::onLoad, this));

	auto saveButton = static_cast<CEGUI::PushButton*>(_menu->getChild("General")->getChild("saveButton"));
	saveButton->subscribeEvent(
		CEGUI::PushButton::EventClicked,
		CEGUI::Event::Subscriber(&MenuScreen::onSave, this));
	//Save Scene Editbox handler:
	_menu->getChild("savePopup")->getChild("saveInput")->subscribeEvent(
		CEGUI::Editbox::EventTextAccepted,
		CEGUI::Event::Subscriber(&MenuScreen::onSaveFinished, this));	

	initRadioButtons();
	initSliders();
	initRGBInputs();
}

void MenuScreen::initRadioButtons() {
	auto dirShadowChecker = static_cast<CEGUI::ToggleButton*>(_menu->getChild("General")->getChild("dirShadow"));
	dirShadowChecker->setSelected(true);
	dirShadowChecker->subscribeEvent(
		CEGUI::ToggleButton::EventSelectStateChanged,
		CEGUI::Event::Subscriber(&MenuScreen::onDShadowToggled, this));

	auto pointShadowChecker = static_cast<CEGUI::ToggleButton*>(_menu->getChild("General")->getChild("pointShadow"));
	pointShadowChecker->setSelected(true);
	pointShadowChecker->subscribeEvent(
		CEGUI::ToggleButton::EventSelectStateChanged,
		CEGUI::Event::Subscriber(&MenuScreen::onPShadowToggled, this));

	auto bloomChecker = static_cast<CEGUI::ToggleButton*>(_menu->getChild("General")->getChild("bloom"));
	bloomChecker->setSelected(true);
	bloomChecker->subscribeEvent(
		CEGUI::ToggleButton::EventSelectStateChanged,
		CEGUI::Event::Subscriber(&MenuScreen::onBloomToggled, this));

	auto debugChecker = static_cast<CEGUI::ToggleButton*>(_menu->getChild("General")->getChild("debugChecker"));
	debugChecker->setSelected(false);
	debugChecker->subscribeEvent(
		CEGUI::ToggleButton::EventSelectStateChanged,
		CEGUI::Event::Subscriber(&MenuScreen::onDebugWinToggled, this));

	auto wireframeChecker = static_cast<CEGUI::ToggleButton*>(_menu->getChild("General")->getChild("wireframeChecker"));
	wireframeChecker->setSelected(false);
	wireframeChecker->subscribeEvent(
		CEGUI::ToggleButton::EventSelectStateChanged,
		CEGUI::Event::Subscriber(&MenuScreen::onWireframeToggled, this));

	auto normalsChecker = static_cast<CEGUI::ToggleButton*>(_menu->getChild("General")->getChild("normalsChecker"));
	normalsChecker->setSelected(false);
	normalsChecker->subscribeEvent(
		CEGUI::ToggleButton::EventSelectStateChanged,
		CEGUI::Event::Subscriber(&MenuScreen::onNormalsToggled, this));

	auto physicsChecker = static_cast<CEGUI::ToggleButton*>(_menu->getChild("General")->getChild("physicsChecker"));
	physicsChecker->setSelected(false);
	physicsChecker->subscribeEvent(
		CEGUI::ToggleButton::EventSelectStateChanged,
		CEGUI::Event::Subscriber(&MenuScreen::onPhysicsToggled, this));
}
void MenuScreen::initSliders() {
	std::vector<std::function<void()>> callbacks = {
		std::bind(&MenuScreen::onGammaChanged, this),
		std::bind(&MenuScreen::onBloomChanged, this),
		std::bind(&MenuScreen::onHdrChanged, this)
	};
	std::vector<std::string> names = {
		"gammaSlider",
		"bloomSlider",
		"hdrSlider"
	};

	for (size_t i = 0; i < names.size(); i++) {
		auto slider = static_cast<CEGUI::Slider*>(_menu->getChild("General")->getChild(names[i]));
		slider->subscribeEvent(CEGUI::Slider::EventValueChanged,
			CEGUI::Event::Subscriber(callbacks[i]));
		slider->setMaxValue(1.0f);
		slider->setClickStep(0.1f);
		_sliders.insert(std::make_pair(names[i], slider));
	}
}

void MenuScreen::initRGBInputs() {
	std::vector<std::string> submenus = {
		"PLight",
		"DLight"
	};

	std::vector<std::function<void()>> callbacks1 = {
		std::bind(&MenuScreen::onPLightDiffuse, this),
		std::bind(&MenuScreen::onPLightSpecular, this),
		std::bind(&MenuScreen::onPLightPosition, this)
	};
	std::vector<std::function<void()>> callbacks2 = {
		std::bind(&MenuScreen::onDLightDiffuse, this),
		std::bind(&MenuScreen::onDLightSpecular, this),
		std::bind(&MenuScreen::onDLightVector, this)
	};
	std::vector<std::string> sliders1 = {
		"pLightDiffuseR",
		"pLightDiffuseG",
		"pLightDiffuseB",
		"pLightSpecularR",
		"pLightSpecularG",
		"pLightSpecularB",
		"pLightPosX",
		"pLightPosY",
		"pLightPosZ"
	};
	std::vector<std::string> sliders2 = {
		"dLightDiffuseR",
		"dLightDiffuseG",
		"dLightDiffuseB",
		"dLightSpecularR",
		"dLightSpecularG",
		"dLightSpecularB",
		"dLightVectorX",
		"dLightVectorY",
		"dLightVectorZ"
	};

	for (size_t i = 0; i < sliders1.size(); i++) {
		auto slider = static_cast<CEGUI::Slider*>(_menu->getChild("PLight")->getChild(sliders1[i]));
		slider->subscribeEvent(
			CEGUI::Slider::EventValueChanged,
			CEGUI::Event::Subscriber(callbacks1[i/3]));
		slider->setMaxValue(1.0f);
		slider->setClickStep(0.1f);
		_values.insert(std::make_pair(sliders1[i], slider));
	}
	for (size_t i = 0; i < sliders2.size(); i++) {
		auto slider = static_cast<CEGUI::Slider*>(_menu->getChild("DLight")->getChild(sliders2[i]));
		slider->subscribeEvent(
			CEGUI::Slider::EventValueChanged,
			CEGUI::Event::Subscriber(callbacks2[i / 3]));
		slider->setMaxValue(1.0f);
		slider->setClickStep(0.1f);
		_values.insert(std::make_pair(sliders2[i], slider));
	}
}

void MenuScreen::updateInput() {
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
				_state = ScreenState::next;
				break;
			}
		}
	}
}

void MenuScreen::onMainClose() {
	auto frame = static_cast<CEGUI::FrameWindow*>(_menu->getChild("General"));
	frame->hide();
}

void MenuScreen::onDLightClose() {
	_menu->getChild("DLight")->hide();
}

void MenuScreen::onSaveClose() {
	_menu->getChild("savePopup")->hide();
}

void MenuScreen::onLoadClose() {
	_menu->getChild("loadPopup")->hide();
}

void MenuScreen::onPLightClose() {
	_menu->getChild("PLight")->hide();
}

bool MenuScreen::onExitClicked(const CEGUI::EventArgs & e) {
	_state = ScreenState::exit;
	return true;
}

bool MenuScreen::onContinueClicked(const CEGUI::EventArgs & e) {
	_state = ScreenState::next;
	return true;
}

bool MenuScreen::onLoad(const CEGUI::EventArgs& e) {
	auto loadWidget = static_cast<CEGUI::FrameWindow*>(_menu->getChild("loadPopup"));
	auto saveWidget = static_cast<CEGUI::FrameWindow*>(_menu->getChild("savePopup"));

	saveWidget->hide();
	loadWidget->show();
	return true;
}

bool MenuScreen::onSave(const CEGUI::EventArgs& e) {
	auto saveWidget = static_cast<CEGUI::FrameWindow*>(_menu->getChild("savePopup"));
	auto loadWidget = static_cast<CEGUI::FrameWindow*>(_menu->getChild("loadPopup"));

	loadWidget->hide();
	saveWidget->show();

	return true;
}

bool MenuScreen::onSaveFinished(const CEGUI::EventArgs& e) {
	//get the text of the input box:
	std::string filename = _menu->getChild("savePopup")->getChild("saveInput")->getText().c_str();	

	//process the string(ie add .xml if it's not already there, add subpath)
	size_t pos = filename.find(".");
	if (pos == std::string::npos) {
		filename += ".xml";
	}
	else if (filename.substr(pos) != ".xml") {
		filename.erase(pos);
		filename += ".xml";
	}
	char chars[] = "()/\\[]{}";
	for (size_t i = 0; i < std::strlen(chars); ++i) {
		filename.erase(std::remove(filename.begin(), filename.end(), chars[i]), filename.end());
	}

	//finally, add our subpath for the savefiles:
	std::string subpath = "SavedScenes/";
	filename = subpath + filename;

	try {
		SceneSaver saver { _appScreen->scene(), filename };
	}
	catch(vitiError e) {
		auto saveLabel = static_cast<CEGUI::FrameWindow*>(_menu->getChild("savePopup")->getChild("saveLabel"));
		saveLabel->setText("Error while saving file.");
		return false;		
	}

	//hide the save dialog again:
	_menu->getChild("savePopup")->hide();	

	return true;
}

bool MenuScreen::onLoadFinished(const CEGUI::EventArgs& e) {

	return true;
}

void MenuScreen::onPhysicsToggled() {
	Physics::instance()->drawDebug();
}

void MenuScreen::onNormalsToggled() {
	_appScreen->_drender.drawNormals();
}

void MenuScreen::onWireframeToggled() {
	_appScreen->_drender.setDrawMode();
}

void MenuScreen::onDebugWinToggled() {
	_appScreen->_drender.drawDebugWin();
}

void MenuScreen::onPShadowToggled() {
	_appScreen->_drender.drawPShadow();
}

void MenuScreen::onDShadowToggled() {
	_appScreen->_drender.drawDShadow();
}

void MenuScreen::onBloomToggled() {
	_appScreen->_drender.applyBloom();
}

void MenuScreen::onDLightDiffuse() {
	dLight* light = _appScreen->_scene.findDLight("dlight");
	if (!light) return;

	glm::vec3 color{};

	color.r = _values["dLightDiffuseR"]->getCurrentValue() * 400.0f / 255.f;
	color.g = _values["dLightDiffuseG"]->getCurrentValue() * 400.0f / 255.f;
	color.b = _values["dLightDiffuseB"]->getCurrentValue() * 400.0f / 255.f;
		
	light->setProperty(lightProps::diffuse, color);
}

void MenuScreen::onDLightSpecular() {
	dLight* light = _appScreen->_scene.findDLight("dlight");
	if (!light) return;

	glm::vec3 color{};

	color.r = _values["dLightSpecularR"]->getCurrentValue() * 400.0f / 255.f;
	color.g = _values["dLightSpecularG"]->getCurrentValue() * 400.0f / 255.f;
	color.b = _values["dLightSpecularB"]->getCurrentValue() * 400.0f / 255.f;

	light->setProperty(lightProps::specular, color);
}

void MenuScreen::onDLightVector() {
	dLight* light = _appScreen->_scene.findDLight("dlight");
	if (!light) return;

	glm::vec3 pos{};

	pos.x = (_values["dLightVectorX"]->getCurrentValue() - 0.5f) * 60.f; //put in range [-30 / 30]
	pos.y = (_values["dLightVectorY"]->getCurrentValue() - 0.5f) * 60.f;
	pos.z = (_values["dLightVectorZ"]->getCurrentValue() -0.5f) * 60.f;

	light->setProperty(lightProps::dir, pos);
}

void MenuScreen::onPLightDiffuse() {
	pLight* light = _appScreen->_scene.findPLight("plight");
	if (!light) return;

	glm::vec3 color{};

	color.r = _values["pLightDiffuseR"]->getCurrentValue() * 400.0f / 255.f;
	color.g = _values["pLightDiffuseG"]->getCurrentValue() * 400.0f / 255.f;
	color.b = _values["pLightDiffuseB"]->getCurrentValue() * 400.0f / 255.f;

	light->setProperty(lightProps::diffuse, color);
}

void MenuScreen::onPLightSpecular() {
	pLight* light = _appScreen->_scene.findPLight("plight");
	if (!light) return;

	glm::vec3 color{};

	color.r = _values["pLightSpecularR"]->getCurrentValue() * 400.0f / 255.f;
	color.g = _values["pLightSpecularG"]->getCurrentValue() * 400.0f / 255.f;
	color.b = _values["pLightSpecularB"]->getCurrentValue() * 400.0f / 255.f;

	light->setProperty(lightProps::specular, color);
}

void MenuScreen::onPLightPosition() {
	pLight* light = _appScreen->_scene.findPLight("plight");
	if (!light) return;

	glm::vec3 pos{};

	pos.x = (_values["pLightPosX"]->getCurrentValue() -0.5f) * 60.f;
	pos.y = (_values["pLightPosY"]->getCurrentValue() - 0.5f) * 60.0f;
	pos.z = (_values["pLightPosZ"]->getCurrentValue() -0.5f) * 60.f;

	_appScreen->_scene["plight"]->transform.setPos(pos);
	light->setProperty(lightProps::pos, pos);
}

void MenuScreen::onGammaChanged() {
	std::cout << "Gamma Slider\n";
	float value = _sliders["gammaSlider"]->getCurrentValue() * 3.0f;
	_appScreen->_drender.setGamma(value);
}

void MenuScreen::onBloomChanged() {
	float value = _sliders["bloomSlider"]->getCurrentValue() * 3.0f;
	_appScreen->_drender.setBloomTreshold(value);
}

void MenuScreen::onHdrChanged() {
	float value = _sliders["hdrSlider"]->getCurrentValue() * 3.0f;
	_appScreen->_drender.setExposure(value);
}

int MenuScreen::getInt(const std::string & widgetName) {
	assert(_values.find(widgetName) != _values.end());

	std::string input = _values[widgetName]->getText().c_str();
	std::istringstream iss{ input };
	int value{};

	if (iss >> value) {		//we have an int
		return value;
	}

	return -100000;
}

float MenuScreen::getFloat(const std::string & widgetName) {
	assert(_values.find(widgetName) != _values.end());

	std::string input = _values[widgetName]->getText().c_str();
	std::istringstream iss{ input };
	float value{};

	if (iss >> std::setprecision(3) >> value) {		//we have a float
		return value;
	}

	return -100000.0f;
}
