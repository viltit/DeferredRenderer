/*	Class for an ingame console that allows to change certain values ingame
	Right now, every command goes back to the IAppScreen::Command function, which
	is not the best solution

	--> apply the Command pattern? How?
*/

#pragma once

#include <CEGUI/CEGUI.h>

#include <string>
#include <deque>

namespace vitiGL {

class GUI;
class IAppScreen;

class Console {
public:
	Console		(IAppScreen* appscreen, GUI* gui, const std::string& layoutFile);
	~Console	();

	void	setText(const std::string& text);
	void	setVisible(bool visible);

	bool	isVisible() { return _window->isVisible(); }

private:
	void	processText(const CEGUI::EventArgs& e);
	void	parseText(CEGUI::String& msg);
	void	writeText(CEGUI::String& msg, CEGUI::Colour = { 0xFFFFFFFF });

	GUI*			_gui;
	IAppScreen*		_appscreen;

	CEGUI::Window*	_window;

	std::deque<CEGUI::ListboxTextItem*> _history;
	int				_historyLength;

	bool			_visible;
};
}