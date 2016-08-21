#include "IApp.hpp"
#include "vitiGL.hpp"

#include "IAppScreen.hpp"
#include "ScreenList.hpp"
#include "Error.hpp"

namespace vitiGL {

IApp::IApp()
	:	_screenList { new ScreenList{ this } },
		_screen		{ nullptr },
		_window		{ nullptr },
		_isRunning	{ false }
{
	vitiGL::start();
}


IApp::~IApp() {
	if (_isRunning) exit();
}


void IApp::run() {
	if (_window == nullptr) throw initError("<App::Run>: Trying to run the app without a window.");

	init();
	onInit(); 

	_isRunning = true;

	while (_isRunning) {
		_window->clear();
		draw();
		_window->swap();
		update();
	}
}

void IApp::init() {
	addScreens();

	_screen = _screenList->current();
	_screen->onEntry();
	_screen->setRunning();
}

void IApp::exit() {
	//onExit();

	_isRunning = false;

	if (_window) {
		delete _window;
		_window = nullptr;
	}
	if (_screenList) {
		delete _screenList;
		_screenList = nullptr;
	}
	vitiGL::stop();
}

void IApp::addWindow(const std::string & name, int width, int height, wStyle style, Color color) {
	_window = new Window(name, width, height, style, color);
}

void IApp::update() {
	if (_screen) {
		switch (_screen->state()) {
		case ScreenState::run:
			_screen->update();
			break;
		case ScreenState::next:
			_screen->onExit();
			_screen = _screenList->next();
			if (_screen) {
				_screen->setRunning();
				_screen->onEntry();
			}
			break;
		case ScreenState::previous:
			_screen->onExit();
			_screen = _screenList->previous();
			if (_screen) {
				_screen->setRunning();
				_screen->onEntry();
			}
			break;
		case ScreenState::exit:
			exit();
			break;
		default:
			break;
		}
	}
	else exit();
}

void IApp::draw() {
	if (_screen && _screen->state() == ScreenState::run) {
		_screen->draw();
	}
}

}