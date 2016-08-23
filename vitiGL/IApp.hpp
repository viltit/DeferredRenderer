#pragma once

#include "Window.hpp"


namespace vitiGL {

class ScreenList;
class IAppScreen;

class IApp {
public:
	IApp();
	virtual ~IApp();

	virtual void run();
	virtual void exit();

	virtual void addWindow	(const std::string& name, 
							 int width, int height, 
							 wStyle style = wStyle::max, 
							 Color color = { 0x00, 0x00, 0x00, 0xFF });

	virtual void addScreens() = 0;

protected:
	/* standard init, cant place in constructor for inheritance reasons: */
	void init();
	/* custom init and exit for derived classed, to be called in their de/constructors: */
	virtual void onInit() = 0;	
	virtual void onExit() = 0;

	/* here happens the interesting stuff: */
	virtual void update();
	virtual void draw();

	IAppScreen* _screen;
	Window*		_window;
	ScreenList* _screenList;

	bool		_isRunning;
};

}