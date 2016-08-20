#include "ScreenList.hpp"

namespace vitiGL {

ScreenList::ScreenList(IApp* app)
	:	_app{ app },
		_current{ SCREEN_INDEX_NONE }
{}


ScreenList::~ScreenList() {
	for (size_t i = 0; i < _screens.size(); i++) {
		if (_screens[i] != nullptr) {
			delete _screens[i];
			_screens[i] = nullptr;
		}
	}
}

IAppScreen* ScreenList::current() {
	if (_current == SCREEN_INDEX_NONE) return nullptr;
	return _screens[_current];
}

IAppScreen* ScreenList::next() {
	IAppScreen* currentScreen = current();
	
	if (currentScreen->next() != SCREEN_INDEX_NONE) {
		_current = currentScreen->next();
	}	
	return current();
}

IAppScreen* ScreenList::previous() {
	IAppScreen* currentScreen = current();

	if (currentScreen->previous() != SCREEN_INDEX_NONE) {
		_current = currentScreen->previous();
	}
	return current();
}

void ScreenList::set(int index) {
	_current = index;	//TO DO: Build safety against setting invalid index
}

void ScreenList::add(IAppScreen* newScreen) {
	newScreen->setIndex(_screens.size());
	_screens.push_back(newScreen);
}

}