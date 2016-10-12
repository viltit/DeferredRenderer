/* Abstract interface for handling different screens (ie. 'Main Menu', 'Game', 'Editor', ...) */

#pragma once

#include <vector>
#include <string>

namespace vitiGL {

class IApp;

const int SCREEN_INDEX_NONE = -1;

enum class ScreenState {
	none,
	run,
	exit,
	next,
	previous
};


class IAppScreen {
public:
	IAppScreen(IApp* app)
		: _state	{ ScreenState::none },
		  _index	{ -1 },
		  _app		{ app }
	{}

	virtual ~IAppScreen() {};

	/* gets called when we switch a screen: */
	virtual void onEntry() = 0;
	virtual void onExit() = 0;

	virtual void update() = 0;
	virtual void draw() = 0;

	virtual int next() const = 0;
	virtual int previous() const = 0;

	virtual std::string command(const std::string& command) = 0; //works with console class

	/* getters ans setters: */
	int index()	const			{ return _index; }
	ScreenState state() const	{ return _state; }
	void setRunning()			{ _state = ScreenState::run; }
	void setIndex(int i)		{ _index = i;  }

protected:
	ScreenState	_state;
	int			_index;
	IApp*		_app;
};
}

