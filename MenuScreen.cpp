#include "MenuScreen.hpp"

#include "App.hpp"
#include "AppScreen.hpp"

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
	/* get lights RGB values and other data:
	dLight* light = _appScreen->_scene.findDLight("dlight");

	if (light) {
		glm::vec3 color = light->ambient();
		_values["dlightAmbientR"]->setText(CEGUI::String(std::to_string(int(color.r * 255.f))));
		_values["dlightAmbientG"]->setText(CEGUI::String(std::to_string(int(color.g * 255.f))));
		_values["dlightAmbientB"]->setText(CEGUI::String(std::to_string(int(color.b * 255.f))));

		color = light->diffuse();
		_values["dlightDiffuseR"]->setText(CEGUI::String(std::to_string(int(color.r * 255.f))));
		_values["dlightDiffuseG"]->setText(CEGUI::String(std::to_string(int(color.g * 255.f))));
		_values["dlightDiffuseB"]->setText(CEGUI::String(std::to_string(int(color.b * 255.f))));

		color = light->specular();
		_values["dlightSpecularR"]->setText(CEGUI::String(std::to_string(int(color.r * 255.f))));
		_values["dlightSpecularG"]->setText(CEGUI::String(std::to_string(int(color.g * 255.f))));
		_values["dlightSpecularB"]->setText(CEGUI::String(std::to_string(int(color.b * 255.f))));

		color = light->dir();
		_values["dlightVectorX"]->setText(CEGUI::String(std::to_string(color.r)));
		_values["dlightVectorY"]->setText(CEGUI::String(std::to_string(color.g)));
		_values["dlightVectorZ"]->setText(CEGUI::String(std::to_string(color.b)));
	}

	pLight* plight = _appScreen->_scene.findPLight("plight");
	if (plight) {
		glm::vec3 color = plight->diffuse();
		_values["plightDiffuseR"]->setText(CEGUI::String(std::to_string(int(color.r * 255.f))));
		_values["plightDiffuseG"]->setText(CEGUI::String(std::to_string(int(color.g * 255.f))));
		_values["plightDiffuseB"]->setText(CEGUI::String(std::to_string(int(color.b * 255.f))));

		color = plight->specular();
		_values["plightSpecularR"]->setText(CEGUI::String(std::to_string(int(color.r * 255.f))));
		_values["plightSpecularG"]->setText(CEGUI::String(std::to_string(int(color.g * 255.f))));
		_values["plightSpecularB"]->setText(CEGUI::String(std::to_string(int(color.b * 255.f))));

		color = plight->pos();
		_values["plightPosX"]->setText(CEGUI::String(std::to_string(color.r)));
		_values["plightPosY"]->setText(CEGUI::String(std::to_string(color.g)));
		_values["plightPosZ"]->setText(CEGUI::String(std::to_string(color.b)));
	}*/

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
		_values.insert(std::make_pair(sliders1[i], slider));
	}
	for (size_t i = 0; i < sliders2.size(); i++) {
		auto slider = static_cast<CEGUI::Slider*>(_menu->getChild("DLight")->getChild(sliders2[i]));
		slider->subscribeEvent(
			CEGUI::Slider::EventValueChanged,
			CEGUI::Event::Subscriber(callbacks2[i / 3]));
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

bool MenuScreen::onExitClicked(const CEGUI::EventArgs & e) {
	_state = ScreenState::exit;
	return true;
}

bool MenuScreen::onContinueClicked(const CEGUI::EventArgs & e) {
	_state = ScreenState::next;
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

	glm::vec3 color{ light->diffuse() };
	int red{ getInt("plightDiffuseR") };
	int green{ getInt("plightDiffuseG") };
	int blue{ getInt("plightDiffuseB") };

	if (red >= 0) color.r = float(red) / 255.f;
	if (green >= 0) color.g = float(green) / 255.f;
	if (blue >= 0) color.b = float(blue) / 255.f;

	light->setProperty(lightProps::diffuse, color);
}

void MenuScreen::onPLightSpecular() {
	pLight* light = _appScreen->_scene.findPLight("plight");
	if (!light) return;

	glm::vec3 color{ light->specular() };
	int red{ getInt("plightSpecularR") };
	int green{ getInt("plightSpecularG") };
	int blue{ getInt("plightSpecularB") };

	if (red >= 0) color.r = float(red) / 255.f;
	if (green >= 0) color.g = float(green) / 255.f;
	if (blue >= 0) color.b = float(blue) / 255.f;

	light->setProperty(lightProps::specular, color);
}

void MenuScreen::onPLightPosition() {
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
