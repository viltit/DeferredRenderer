#pragma once
#include "IAppScreen.hpp"

#include "vitiGL.hpp"
#include <map>

class App;
class AppScreen;

class MenuScreen : public vitiGL::IAppScreen {
public:
	MenuScreen(App* app, AppScreen* appScreen, vitiGL::Window* window);
	~MenuScreen();

	// Inherited via IAppScreen
	virtual void onEntry() override;
	virtual void onExit() override;

	virtual void update() override;
	virtual void draw() override;

	virtual int next() const override;
	virtual int previous() const override;

private:
	void initGUI();
	void initRadioButtons();
	void initSliders();
	void initRGBInputs();

	void updateInput();

	/* event handlers: */
	bool onExitClicked(const CEGUI::EventArgs& e);
	bool onContinueClicked(const CEGUI::EventArgs& e);

	void onPhysicsToggled();
	void onNormalsToggled();
	void onWireframeToggled();
	void onDebugWinToggled();
	void onPShadowToggled();
	void onDShadowToggled();
	void onBloomToggled();

	void onDLightDiffuse();
	void onDLightSpecular();
	void onDLightVector();
	void onPLightDiffuse();
	void onPLightSpecular();
	void onPLightPosition();

	void onGammaChanged();
	void onBloomChanged();
	void onHdrChanged();

	/* Convert Cegui-String from editbox to an int: */
	int getInt(const std::string& widgetName);
	float getFloat(const std::string& widgetName);

	CEGUI::Window*		_menu;
	vitiGL::Window*		_window;

	AppScreen*			_appScreen;
	vitiGL::GUI			_gui;
	vitiGL::Timer		_timer;

	/* this maps stores all editboxes that can change the scenes values: */
	std::map<std::string, CEGUI::Slider*> _values;

	/* this map stores all sliders: */
	std::map<std::string, CEGUI::Slider*> _sliders;

	CEGUI::PushButton*	_quitButton;
};

