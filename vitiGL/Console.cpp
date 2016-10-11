#include "Console.hpp"

#include "GUI.hpp"
#include "Error.hpp"

namespace vitiGL {

Console::Console(GUI* gui, const std::string& layoutFile)
	:	_gui		{ gui },
		_visible	{ true }
{ 
	if (!gui) throw vitiError("<Console::Console> Tried to initialize a console with a nullpointer GUI");

	_window = _gui->createConsole(layoutFile);
	_window->getChild("Editbox")->subscribeEvent(CEGUI::Editbox::EventTextAccepted,
		CEGUI::Event::Subscriber(&Console::processText, this));
}

 
Console::~Console() { 
}

void Console::setText(const std::string & text) {
	CEGUI::Listbox* listbox = static_cast<CEGUI::Listbox*>(_window->getChild("History"));
	listbox->appendText(CEGUI::String(text));
}

void Console::setVisible(bool visible) {
	_window->setVisible(visible);
	_visible = visible;

	CEGUI::Editbox* editbox = static_cast<CEGUI::Editbox*>(_window->getChild("Editbox"));
	if (visible) editbox->activate();
	else editbox->deactivate();
}

void Console::processText(const CEGUI::EventArgs & e) {
	CEGUI::String msg = _window->getChild("Editbox")->getText();
	parseText(msg);
	_window->getChild("Editbox")->setText("");
}


void Console::parseText(CEGUI::String & msg) {
	std::string input = msg.c_str();

	if (input.length() == 0) return;

	std::string output{};
	if (input == "hide") setVisible(false);
	
	//process more commands! (callbacks?)

	else output = "<" + input + "> is an invalid command.";

	writeText(CEGUI::String(output), CEGUI::Colour(1.0f, 0.0f, 0.0f));
}

void Console::writeText(CEGUI::String & msg, CEGUI::Colour color) {
	CEGUI::Listbox* outputWindow = static_cast<CEGUI::Listbox*>(_window->getChild("History"));
	
	/* the actual text is held by a ListboxTextItem: */
	CEGUI::ListboxTextItem* newLine = new CEGUI::ListboxTextItem(msg); //need to delete??
	newLine->setTextColours(color);

	/* finally, add the text to our box: */
	outputWindow->addItem(newLine);
}


}