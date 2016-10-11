#pragma once

#include <CEGUI/CEGUI.h>

#include <string>

namespace vitiGL {

class GUI;

class Console {
public:
	Console		(GUI* gui, const std::string& layoutFile);
	~Console	();

	void	setVisible(bool visible) { _visible = visible; }

private:
	void	processText(const CEGUI::EventArgs& e);
	void	parseText(CEGUI::String& msg);

	GUI*			_gui;
	CEGUI::Window*	_window;

	bool			_visible;
};
}