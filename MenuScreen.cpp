#include "MenuScreen.hpp"

#include "App.hpp"
#include "AppScreen.hpp"

#include <Physics.hpp>

using namespace vitiGL;
using namespace vitiGEO;

MenuScreen::MenuScreen(App* app, AppScreen* appScreen, vitiGL::Window* window) 
	:	IAppScreen	{ app },
		_appScreen  { appScreen },
		_gui		{ "GUI", "AlfiskoSkin.scheme" },
		_shader		{ "Shaders/simple.vert.glsl", "Shaders/simple.frag.glsl" }
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
	std::cout << "MenuScreen::onEntry\n";
}

void MenuScreen::onExit() {
	std::cout << "MenuScreen::onExit\n";
}

void MenuScreen::update() {
	updateInput();
	_appScreen->updateFreezed();
	_gui.update(_timer.frame_time());
	std::cout << "MenuScreen::update\n";
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
	/* this would maybe better be done in an cegui-layout xml-file */
	_gui.setFont("DejaVuSans-10");
	_gui.setMouseCursor("AlfiskoSkin/MouseArrow");

	/* initialize widgets: */
	glm::vec2 pos{ 0.02, 0.03 };
	glm::vec2 sizeA{ 0.02f, 0.02f };
	glm::vec2 sizeB{ 0.15, 0.02f };
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

	/* add sliders: */
	auto dLightR = static_cast<CEGUI::Editbox*>(
		_gui.createWidget(glm::vec4{ pos.x, pos.y += 2.f*sizeA.y, sizeA }, glm::vec4{}, "AlfiskoSkin/Editbox", "dLightR"));
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