#include "App.hpp"


App::App() 
{}

App::App(const std::string windowName, int width, int height, vitiGL::wStyle style, vitiGL::Color color) {
	addWindow(windowName, width, height, style, color);
}

App::~App() {
}

void App::onInit(){
}

void App::onExit() {
}

void App::addScreens() {
	_screen = new AppScreen(this, _window);
	_screenList->add(_screen);
	_screenList->set(_screen->index());
}