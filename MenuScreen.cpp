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
	//initGUI();
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
	}

	_sliders["gammaSlider"]->setCurrentValue(_appScreen->_drender.gamma() / 3.0f);
	_sliders["bloomSlider"]->setCurrentValue(_appScreen->_drender.bloomTreshold() / 3.0f);
	_sliders["hdrSlider"]->setCurrentValue(_appScreen->_drender.exposure() / 3.0f); */
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
	glm::vec2 pos{};
	pos = initRadioButtons(glm::vec2{ 0.02f, 0.03f }, glm::vec2{ 0.02f, 0.02f }, glm::vec2{ 0.15f, 0.02f });
	pos.y += 0.1f;
	pos = initSliders(pos, glm::vec2{ 0.09, 0.03 }, glm::vec2{ 0.15, 0.02 });
	pos = initRGBInputs(glm::vec2{ 0.8, 0.03 }, glm::vec2{ 0.03f, 0.03f }, glm::vec2{ 0.15f, 0.02f });

	//Add a Quit Button
	auto quitButton = static_cast<CEGUI::PushButton*>(
		_gui.createWidget(glm::vec4{ 0.8f, 0.8f, 0.1f, 0.05f }, glm::vec4{}, "AlfiskoSkin/Button", "quitButton"));
	quitButton->setText(CEGUI::String("Quit"));
	quitButton->subscribeEvent(
		CEGUI::PushButton::EventClicked,
		CEGUI::Event::Subscriber(&MenuScreen::onExitClicked, this));
}

glm::vec2 MenuScreen::initRadioButtons(const glm::vec2& startPos, const glm::vec2& buttonSize, const glm::vec2& textSize) {
	glm::vec2 pos = startPos;
	glm::vec2 sizeA = buttonSize;
	glm::vec2 sizeB = textSize;
	std::vector<CEGUI::DefaultWindow*> labels;

	/* dir Shadow Checker with Label */
	auto dirShadowChecker = static_cast<CEGUI::ToggleButton*>(
		_gui.createWidget(glm::vec4{ pos.x, pos.y += 2.f*sizeA.y, sizeA }, glm::vec4{}, "AlfiskoSkin/Checkbox", "dirShadow"));
	dirShadowChecker->setSelected(true);
	dirShadowChecker->subscribeEvent(
		CEGUI::ToggleButton::EventSelectStateChanged,
		CEGUI::Event::Subscriber(&MenuScreen::onDShadowToggled, this));

	auto dirShadowLabel = static_cast<CEGUI::DefaultWindow*>(
		_gui.createWidget(glm::vec4{ pos.x + sizeA.x, pos.y, sizeB }, glm::vec4{}, "AlfiskoSkin/Label", "dirShadowLabel"));
	dirShadowLabel->setText("Directional Light Shadows");
	labels.push_back(dirShadowLabel);

	/* point Shadow Checker */
	auto pointShadowChecker = static_cast<CEGUI::ToggleButton*>(
		_gui.createWidget(glm::vec4{ pos.x, pos.y += 2.f*sizeA.y, sizeA }, glm::vec4{}, "AlfiskoSkin/Checkbox", "pointShadow"));
	pointShadowChecker->setSelected(true);
	pointShadowChecker->subscribeEvent(
		CEGUI::ToggleButton::EventSelectStateChanged,
		CEGUI::Event::Subscriber(&MenuScreen::onPShadowToggled, this));

	auto pointShadowLabel = static_cast<CEGUI::DefaultWindow*>(
		_gui.createWidget(glm::vec4{ pos.x + sizeA.x, pos.y, sizeB }, glm::vec4{}, "AlfiskoSkin/Label", "pointShadowLabel"));
	pointShadowLabel->setText("Point Light Shadows");
	labels.push_back(pointShadowLabel);

	/* bloom Checker --> needs rework in the renderer for performance gain when its off */
	auto bloomChecker = static_cast<CEGUI::ToggleButton*>(
		_gui.createWidget(glm::vec4{ pos.x, pos.y += 2.f*sizeA.y, sizeA }, glm::vec4{}, "AlfiskoSkin/Checkbox", "bloom"));
	bloomChecker->setSelected(true);
	bloomChecker->subscribeEvent(
		CEGUI::ToggleButton::EventSelectStateChanged,
		CEGUI::Event::Subscriber(&MenuScreen::onBloomToggled, this));

	auto bloomLabel = static_cast<CEGUI::DefaultWindow*>(
		_gui.createWidget(glm::vec4{ pos.x + sizeA.x, pos.y, sizeB }, glm::vec4{}, "AlfiskoSkin/Label", "bloomLabel"));
	bloomLabel->setText("Bloom");
	labels.push_back(bloomLabel);

	/* Debug window checker: */
	auto debugChecker = static_cast<CEGUI::ToggleButton*>(
		_gui.createWidget(glm::vec4{ pos.x, pos.y += 2.f*sizeA.y, sizeA }, glm::vec4{}, "AlfiskoSkin/Checkbox", "debugChecker"));
	debugChecker->setSelected(false);
	debugChecker->subscribeEvent(
		CEGUI::ToggleButton::EventSelectStateChanged,
		CEGUI::Event::Subscriber(&MenuScreen::onDebugWinToggled, this));

	auto debugLabel = static_cast<CEGUI::DefaultWindow*>(
		_gui.createWidget(glm::vec4{ pos.x + sizeA.x, pos.y, sizeB }, glm::vec4{}, "AlfiskoSkin/Label", "debugLabel"));
	debugLabel->setText("Debug Windows");
	labels.push_back(debugLabel);

	/* Wireframe checker: */
	auto wireframeChecker = static_cast<CEGUI::ToggleButton*>(
		_gui.createWidget(glm::vec4{ pos.x, pos.y += 2.f*sizeA.y, sizeA }, glm::vec4{}, "AlfiskoSkin/Checkbox", "wireframeChecker"));
	wireframeChecker->setSelected(false);
	wireframeChecker->subscribeEvent(
		CEGUI::ToggleButton::EventSelectStateChanged,
		CEGUI::Event::Subscriber(&MenuScreen::onWireframeToggled, this));

	auto wireframeLabel = static_cast<CEGUI::DefaultWindow*>(
		_gui.createWidget(glm::vec4{ pos.x + sizeA.x, pos.y, sizeB }, glm::vec4{}, "AlfiskoSkin/Label", "wireframeLabel"));
	wireframeLabel->setText("Draw Wireframes");
	labels.push_back(wireframeLabel);

	/* Normals checker: */
	auto normalsChecker = static_cast<CEGUI::ToggleButton*>(
		_gui.createWidget(glm::vec4{ pos.x, pos.y += 2.f*sizeA.y, sizeA }, glm::vec4{}, "AlfiskoSkin/Checkbox", "normalsChecker"));
	normalsChecker->setSelected(false);
	normalsChecker->subscribeEvent(
		CEGUI::ToggleButton::EventSelectStateChanged,
		CEGUI::Event::Subscriber(&MenuScreen::onNormalsToggled, this));

	auto normalsLabel = static_cast<CEGUI::DefaultWindow*>(
		_gui.createWidget(glm::vec4{ pos.x + sizeA.x, pos.y, sizeB }, glm::vec4{}, "AlfiskoSkin/Label", "normalsLabel"));
	normalsLabel->setText("Draw Normals");
	labels.push_back(normalsLabel);

	/* Physics checker: */
	auto physicsChecker = static_cast<CEGUI::ToggleButton*>(
		_gui.createWidget(glm::vec4{ pos.x, pos.y += 2.f*sizeA.y, sizeA }, glm::vec4{}, "AlfiskoSkin/Checkbox", "physicsChecker"));
	physicsChecker->setSelected(false);
	physicsChecker->subscribeEvent(
		CEGUI::ToggleButton::EventSelectStateChanged,
		CEGUI::Event::Subscriber(&MenuScreen::onPhysicsToggled, this));

	auto physicsLabel = static_cast<CEGUI::DefaultWindow*>(
		_gui.createWidget(glm::vec4{ pos.x + sizeA.x, pos.y, sizeB }, glm::vec4{}, "AlfiskoSkin/Label", "physicsLabel"));
	physicsLabel->setText("Draw Physics Wireframe");
	labels.push_back(physicsLabel);

	/* set Formatting and color for all labels: */
	for (auto& L : labels) {
		L->setProperty("HorzFormatting", "LeftAligned");
		L->setProperty("NormalTextColour", "ffaaaaaa");
	}

	return pos;
}

glm::vec2 MenuScreen::initSliders(const glm::vec2 & startPos, const glm::vec2 & sliderSize, const glm::vec2 & textSize) {
	glm::vec2 pos = startPos;
	glm::vec2 sizeA = sliderSize;
	glm::vec2 sizeB = textSize;
	std::vector<CEGUI::DefaultWindow*> labels;

	std::vector<std::string> labelNames = {
		"gammaSLabel",
		"bloomSLabel",
		"dhrSLabel"
	};
	std::vector<std::string> labelText = {
		"Gamma adjustment",
		"Bloom Treshold",
		"HDR Exposure Exponent"
	};
	std::vector<std::string> sliderNames {
		"gammaSlider",
		"bloomSlider",
		"hdrSlider"
	};

	std::vector<std::function<void()>> callbacks = {
		std::bind(&MenuScreen::onGammaChanged, this),
		std::bind(&MenuScreen::onBloomChanged, this),
		std::bind(&MenuScreen::onHdrChanged, this)
	};

	/* add sliders: */
	for (size_t i = 0; i < labelNames.size(); i++) {
		auto label = static_cast<CEGUI::DefaultWindow*>(
			_gui.createWidget(glm::vec4{ pos.x, pos.y += sizeA.y, sizeB }, glm::vec4{}, "AlfiskoSkin/Label", labelNames[i]));
		label->setText(labelText[i]);
		labels.push_back(label);
		auto slider = static_cast<CEGUI::Slider*>(
			_gui.createWidget(glm::vec4{ pos.x, pos.y += sizeA.y, sizeA }, glm::vec4{}, "AlfiskoSkin/HorizontalSlider", sliderNames[i]));
		slider->subscribeEvent(
			CEGUI::Slider::EventValueChanged,
			CEGUI::Event::Subscriber(callbacks[i]));
		slider->setMaxValue(1.0f);
		slider->setClickStep(0.1f);
		_sliders.insert(std::make_pair(sliderNames[i], slider));
	}

	/* set Formatting and color for all labels: */
	for (auto& L : labels) {
		L->setProperty("HorzFormatting", "LeftAligned");
		L->setProperty("NormalTextColour", "ffaaaaaa");
	}

	return pos;
}

glm::vec2 MenuScreen::initRGBInputs(const glm::vec2& startPos, const glm::vec2& buttonSize, const glm::vec2& textSize) {
	glm::vec2 pos = startPos;
	glm::vec2 sizeA = buttonSize;
	glm::vec2 sizeB = textSize;
	std::vector<CEGUI::DefaultWindow*> labels;

	/* Store this somewhere else, maybe in a textfile? */
	std::vector<std::string> labelNames = {
		"dLightAmbientLabel",
		"dLightDiffuseLabel",
		"dLightSpecularLabel",
		"dLightVectorLabel",
		"pLightDiffuseLabel",
		"pLightSpecularLabel",
		"pLightPositionLabel"
	};
	std::vector<std::string> labelText = {
		"Direction Light Ambient Color",
		"Directional Light Diffuse Color",
		"Directional Light Specular Color",
		"Directional Lights Vector",
		"Point Light Diffuse Color",
		"Point Light Specular Color",
		"Point Light Position"
	};

	std::vector<std::function<void()>> callbacks = {
		std::bind(&MenuScreen::onDLightAmbient, this),
		std::bind(&MenuScreen::onDLightDiffuse, this),
		std::bind(&MenuScreen::onDLightSpecular, this),
		std::bind(&MenuScreen::onDLightVector, this),
		std::bind(&MenuScreen::onPLightDiffuse, this),
		std::bind(&MenuScreen::onPLightSpecular, this),
		std::bind(&MenuScreen::onPLightPosition, this)
	};

	std::vector<std::string> editboxes = {
		"dlightAmbientR",
		"dlightAmbientG",
		"dlightAmbientB",
		"dlightDiffuseR",
		"dlightDiffuseG",
		"dlightDiffuseB",
		"dlightSpecularR",
		"dlightSpecularG",
		"dlightSpecularB",
		"dlightVectorX",
		"dlightVectorY",
		"dlightVectorZ",
		"plightDiffuseR",
		"plightDiffuseG",
		"plightDiffuseB",
		"plightSpecularR",
		"plightSpecularG",
		"plightSpecularB",
		"plightPosX",
		"plightPosY",
		"plightPosZ"
	};

	for (size_t i = 0; i < labelNames.size(); i++) {
		auto label = static_cast<CEGUI::DefaultWindow*>(
			_gui.createWidget(glm::vec4{ pos.x, pos.y = pos.y + buttonSize.y, sizeB }, glm::vec4{}, "AlfiskoSkin/Label", labelNames[i]));
		label->setText(labelText[i]);
		labels.push_back(label);
		pos.y += textSize.y;

		glm::vec2 rowPos = pos;
		for (int j = 3 * i; j < 3 * i + 3; j++) {
			auto dlightDiffuseR = static_cast<CEGUI::Editbox*>(
				_gui.createWidget(
					glm::vec4{ rowPos.x, pos.y = pos.y, sizeA.x, sizeA.y },
					glm::vec4{}, "AlfiskoSkin/Editbox", editboxes[j]));
			dlightDiffuseR->subscribeEvent(
				CEGUI::Editbox::EventTextChanged,
				CEGUI::Event::Subscriber(callbacks[i]));
			_values.insert(std::make_pair(editboxes[j], dlightDiffuseR));
			rowPos.x += buttonSize.x;
		}
	}
	/* set Formatting and color for all labels: */
	for (auto& L : labels) {
		L->setProperty("HorzFormatting", "CenterAligned");
		L->setProperty("NormalTextColour", "ffaaaaaa");
	}

	return pos;
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

void MenuScreen::onDLightAmbient() {
	dLight* light = _appScreen->_scene.findDLight("dlight");
	if (!light) return;

	glm::vec3 color{ light->ambient() };
	int red{ getInt("dlightAmbientR") };
	int green{ getInt("dlightAmbientG") };
	int blue{ getInt("dlightAmbientB") };

	if (red >= 0) color.r = float(red) / 255.f;
	if (green >= 0) color.g = float(green) / 255.f;
	if (blue >= 0) color.b = float(blue) / 255.f;

	light->setProperty(lightProps::ambient, color);
}

void MenuScreen::onDLightDiffuse() {
	dLight* light = _appScreen->_scene.findDLight("dlight");
	if (!light) return;

	glm::vec3 color{ light->diffuse() };
	int red		{ getInt("dlightDiffuseR") };
	int green	{ getInt("dlightDiffuseG") };
	int blue	{ getInt("dlightDiffuseB") };

	if (red >= 0) color.r = float(red) / 255.f;
	if (green >= 0) color.g = float(green) / 255.f;
	if (blue >= 0) color.b = float(blue) / 255.f;
		
	light->setProperty(lightProps::diffuse, color);
}

void MenuScreen::onDLightSpecular() {
	dLight* light = _appScreen->_scene.findDLight("dlight");
	if (!light) return;

	glm::vec3 color{ light->specular() };
	int red{ getInt("dlightSpecularR") };
	int green{ getInt("dlightSpecularG") };
	int blue{ getInt("dlightSpecularB") };

	if (red >= 0) color.r = float(red) / 255.f;
	if (green >= 0) color.g = float(green) / 255.f;
	if (blue >= 0) color.b = float(blue) / 255.f;

	light->setProperty(lightProps::specular, color);
}

void MenuScreen::onDLightVector() {
	dLight* light = _appScreen->_scene.findDLight("dlight");
	if (!light) return;

	glm::vec3 dir{ light->dir() };
	float x{ getFloat("dlightVectorX") };
	float y{ getFloat("dlightVectorY") };
	float z{ getFloat("dlightVectorZ") };

	if (x > -100000) dir.x = x;
	if (y > -100000) dir.y = y;
	if (z > -100000) dir.z = z;

	dir = glm::normalize(dir);
	light->setProperty(lightProps::dir, dir);
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
