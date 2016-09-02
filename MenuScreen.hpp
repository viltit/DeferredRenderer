#pragma once
#include "IAppScreen.hpp"

#include "vitiGL.hpp"

class App;

class MenuScreen : public vitiGL::IAppScreen {
public:
	MenuScreen(App* app, vitiGL::Window* window);
	~MenuScreen();

	// Inherited via IAppScreen
	virtual void onEntry() override;
	virtual void onExit() override;

	virtual void update() override;
	virtual void draw() override;

	virtual int next() const override;
	virtual int previous() const override;

private:
	void updateInput();

	/* event handlers: */
	bool MenuScreen::onExitClicked(const CEGUI::EventArgs& e);
	bool MenuScreen::onContinueClicked(const CEGUI::EventArgs& e);

	bool onBloomSlider();
	bool onGammaSlider();

	//vitiGL::glRenderer  _renderer; no need for this?
	vitiGL::GUI			_gui;
	vitiGL::sQuad		_screenQuad;
	vitiGL::Shader		_shader;
	vitiGL::Timer		_timer;

	CEGUI::Slider*		_bloomSlider;
	CEGUI::Slider*		_gammaSlider;

	//std::vector<CEGUI::PushButton*> _buttons;
};

