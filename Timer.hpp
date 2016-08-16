#pragma once

#include <SDL2/SDL.h>

namespace vitiGL {

class Timer {
public:
	Timer();
	~Timer();

	void		on();
	void		off();
	void		pause();
	void		unpause();

	Uint32		get_time();		/* returns the time elapsed since the timer is on */
	Uint32		frame_time();	/* returns the time elapsed since the last call */

	bool		is_on()			{ return _is_on; }
	bool		is_paused()		{ return _is_paused; }

private:
	Uint32		_start_t;
	Uint32		_paused_t;
	Uint32		_frame_t;
	bool		_is_paused;
	bool		_is_on;

};

}