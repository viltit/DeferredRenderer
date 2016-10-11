#include "Console.hpp"

#include "GUI.hpp"
#include "Error.hpp"

namespace vitiGL {

Console::Console(GUI* gui, const std::string& layoutFile)
	:	_gui		{ gui },
		_visible	{ false }
{ 
	if (!gui) throw vitiError("<Console::Console> Tried to initialize a console with a nullpointer GUI");

	_window = _gui->createConsole(layoutFile);
	_window->getChild("Editbox")->subscribeEvent(CEGUI::Editbox::EventTextAccepted,
		CEGUI::Event::Subscriber(&Console::processText, this));
}


Console::~Console() { 
}

void Console::processText(const CEGUI::EventArgs & e) {

	/* get the tex in the editbox: */
	CEGUI::String msg = _window->getChild("Editbox")->getText();
	parseText(msg);
}


void Console::parseText(CEGUI::String & msg) {
}


}