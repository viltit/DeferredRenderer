#pragma once

#include <CEGUI/CEGUI.h>

#include <string>

namespace vitiGL {

class GUI;

class Console {
public:
	Console		(GUI* gui, const std::string& layoutFile);
	~Console	();

	void	setText(const std::string& text);
	void	setVisible(bool visible);

	bool	isVisible() { return _window->isVisible(); }

private:
	void	processText(const CEGUI::EventArgs& e);
	void	parseText(CEGUI::String& msg);
	void	writeText(CEGUI::String& msg, CEGUI::Colour = { 0xFFFFFFFF });

	GUI*			_gui;
	CEGUI::Window*	_window;

	bool			_visible;
};
}