#pragma once

#include <vector>
#include "IAppScreen.hpp"
#include "IApp.hpp"


namespace vitiGL {

class ScreenList {
public:
	ScreenList(IApp* app);
	virtual ~ScreenList();

	IAppScreen* current();
	IAppScreen* next();
	IAppScreen* previous();

	void set(int index);
	void add(IAppScreen* newScreen);

private:
	IApp*	_app;
	std::vector<IAppScreen*> _screens;
	int _current;
};
}