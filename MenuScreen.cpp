#include "MenuScreen.hpp"

#include "App.hpp"
#include "AppScreen.hpp"

#include <sstream>
#include <Physics.hpp>

using namespace vitiGL;
using namespace vitiGEO;

MenuScreen::MenuScreen(App* app, AppScreen* appScreen, vitiGL::Window* window) 
	:	IAppScreen	{ app },
		_appScreen  { appScreen },
		_gui		{ "GUI", "AlfiskoSkin.scheme" }
{
	assert(_appScreen);

	_index = SCREEN_INDEX_MENU;

	/* Initialize gui elements: */
	_gui.setScheme("AlfiskoSkin.scheme");
	_gui.setFont("DejaVuSans-10");
	_gui.setMouseCursor("AlfiskoSkin/MouseArrow");

	SDL_ShowCursor(0);

	initGUI();
}


MenuScreen::~MenuScreen() {
}

void MenuScreen::onEntry() {
	/* get lights RGB values and other data: */
	dLight* light = _appScreen->_scene.findDLight("dlight");

	if (light) {
		glm::vec3 color = light->diffuse();
		std::cout << color << std::endl;
		std::cout << int(color.r * 255.f) << std::endl;
		std::cout << std::to_string(int(color.r * 255.f)) << std::endl;
		_values["dlightDiffuseR"]->setText(CEGUI::String(std::to_string(int(color.r * 255.f))));
		_values["dlightDiffuseG"]->setText(CEGUI::String(std::to_string(int(color.g * 255.f))));
		_values["dlightDiffuseB"]->setText(CEGUI::String(std::to_string(int(color.b * 255.f))));
	}
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
	pos = initRGBInputs(pos, glm::vec2{ 0.04f, 0.04f }, glm::vec2{ 0.15f, 0.02f });

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
		_gui.createWidget(glm::vec4{ pos.x, pos.y, sizeA }, glm::vec4{}, "AlfiskoSkin/Checkbox", "dirShadow"));
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

	/* add sliders:
	auto gammaSlider = static_cast<CEGUI::Slider*>(
	_gui.createWidget(glm::vec4{ pos.x, pos.y += 2.f*sizeA.y, sizeA }, glm::vec4{}, "AlfiskoSkin/HorizontalSlider", "gammaSlider"));

	auto gammaLabel = static_cast<CEGUI::DefaultWindow*>(
	_gui.createWidget(glm::vec4{ pos.x + sizeA.x, pos.y, sizeB }, glm::vec4{}, "AlfiskoSkin/Label", "gammaLabel"));
	gammaLabel->setText("Gamma adjustment");*/

	return pos;
}

glm::vec2 MenuScreen::initRGBInputs(const glm::vec2& startPos, const glm::vec2& buttonSize, const glm::vec2& textSize) {
	glm::vec2 pos = startPos;
	glm::vec2 sizeA = buttonSize;
	glm::vec2 sizeB = textSize;
	std::vector<CEGUI::DefaultWindow*> labels;

	/* Direction lights Diffuse Color: */
	auto dLightDiffuseLabel = static_cast<CEGUI::DefaultWindow*>(
		_gui.createWidget(glm::vec4{ pos.x, pos.y = pos.y + buttonSize.y, sizeB }, glm::vec4{}, "AlfiskoSkin/Label", "dLightDiffuseLabel"));
	dLightDiffuseLabel->setText("Directional Light Diffuse Color:");
	labels.push_back(dLightDiffuseLabel);

	auto dlightDiffuseR = static_cast<CEGUI::Editbox*>(
		_gui.createWidget(
			glm::vec4{ pos.x, pos.y = pos.y + textSize.y, sizeA.x, sizeA.y },
			glm::vec4{}, "AlfiskoSkin/Editbox", "dLightDiffuseR"));
	dlightDiffuseR->subscribeEvent(
		CEGUI::Editbox::EventTextAccepted,
		CEGUI::Event::Subscriber(&MenuScreen::onDLightDiffuse, this));
	_values.insert(std::make_pair("dlightDiffuseR", dlightDiffuseR));

	auto dlightDiffuseG = static_cast<CEGUI::Editbox*>(
		_gui.createWidget(
			glm::vec4{ pos.x + buttonSize.x, pos.y = pos.y, sizeA.x, sizeA.y },
			glm::vec4{}, "AlfiskoSkin/Editbox", "dLightDiffuseG"));
	dlightDiffuseG->subscribeEvent(
		CEGUI::Editbox::EventTextChanged,
		CEGUI::Event::Subscriber(&MenuScreen::onDLightDiffuse, this));
	_values.insert(std::make_pair("dlightDiffuseG", dlightDiffuseG));

	auto dlightDiffuseB = static_cast<CEGUI::Editbox*>(
		_gui.createWidget(
			glm::vec4{ pos.x + 2.f*buttonSize.x, pos.y = pos.y, sizeA.x, sizeA.y },
			glm::vec4{}, "AlfiskoSkin/Editbox", "dLightDiffuseB"));
	dlightDiffuseB->subscribeEvent(
		CEGUI::Editbox::EventTextChanged,
		CEGUI::Event::Subscriber(&MenuScreen::onDLightDiffuse, this));
	_values.insert(std::make_pair("dlightDiffuseB", dlightDiffuseB));

	/* Directional Lights Direction: */


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

int MenuScreen::getInt(const std::string & widgetName) {
	assert(_values.find(widgetName) != _values.end());

	std::string input = _values[widgetName]->getText().c_str();
	std::istringstream iss{ input };
	int value{};

	if (iss >> value) {		//we have an int
		return value;
	}

	return -1;
}
