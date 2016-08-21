#include "MenuScreen.hpp"

#include "App.hpp"
using namespace vitiGL;

MenuScreen::MenuScreen(App* app, vitiGL::Window* window) 
	:	IAppScreen	{ app },
		_gui		{ "GUI" },
		_shader		{ "Shaders/simple.vert.glsl", "Shaders/simple.frag.glsl" }
{
	_index = SCREEN_INDEX_MENU;

	/* Initialize gui elements: */
	_gui.setScheme("AlfiskoSkin.scheme");
	_gui.setFont("DejaVuSans-10");
	_gui.setMouseCursor("AlfiskoSkin/MouseArrow");

	SDL_ShowCursor(0);

	/* Continue button: */
	auto* cButton = static_cast<CEGUI::PushButton*>(
		_gui.createWidget(glm::vec4{ 0.45f, 0.4f, 0.1f, 0.05f }, glm::vec4{}, "AlfiskoSkin/Button", "ContinueButton"));
	cButton->setText("Continue");
	cButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuScreen::onContinueClicked, this));

	/* Exit Button: */
	auto* eButton = static_cast<CEGUI::PushButton*>(
		_gui.createWidget(glm::vec4{ 0.45f, 0.46f, 0.1f, 0.05f }, glm::vec4{}, "AlfiskoSkin/Button", "ExitButton"));
	eButton->setText("Exit");
	eButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuScreen::onExitClicked, this));
}


MenuScreen::~MenuScreen() {
}

void MenuScreen::onEntry() {

}

void MenuScreen::onExit() {
}

void MenuScreen::update() {
	updateInput();
	_gui.update(_timer.frame_time());
}

void MenuScreen::draw() {
	_screenQuad.draw(_shader, globals::screenshot, "tex");
	_gui.draw();
}

int MenuScreen::next() const {
	return SCREEN_INDEX_APP;
}

int MenuScreen::previous() const {
	return SCREEN_INDEX_NONE;
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
				_state = ScreenState::exit;
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
