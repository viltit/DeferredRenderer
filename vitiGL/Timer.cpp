#include "Timer.hpp"

namespace vitiGL {

Timer::Timer() {
	off();
}


Timer::~Timer() {
}


void Timer::on() {
	_is_on = true;
	_is_paused = false;
	_start_t = SDL_GetTicks();
	_paused_t = 0;
	_frame_t = 0;
	_fpsTime = 0;
	_accTime = 0;
	_fps = 0;
}


void Timer::off() {
	_start_t = 0;
	_paused_t = 0;
	_frame_t = 0;
	_is_on = false;
	_is_paused = false;
}


void Timer::pause() {
	if (!_is_on || _is_paused)
		return;
	_is_paused = true;
	_paused_t = SDL_GetTicks() - _start_t;
	_start_t = 0;
	_frame_t = 0;
}


void Timer::unpause() {
	if (!_is_on || !_is_paused)
		return;
	_is_paused = false;
	_start_t = SDL_GetTicks() - _paused_t;
	_paused_t = 0;
	_frame_t = get_time();
}


Uint32 Timer::get_time() {
	if (!_is_on)
		return 0;
	if (_is_paused)
		return _paused_t;
	return (SDL_GetTicks() - _start_t);
}

Uint32 Timer::frame_time() {
	if (!_is_on || _is_paused)
		return 0;
	Uint32 temp = get_time() - _frame_t;
	_frame_t = get_time();
	return temp;
}

int Timer::fps() {
	if (!_is_on || _is_paused) return _fps;
	Uint32 deltaTime = get_time() - _fpsTime;
	_fpsTime = get_time();
	_accTime += deltaTime;
	_fpsLoops++;

	if (_fpsLoops > 30) {
		_fps = 1000 * _fpsLoops / _accTime;
		_fpsLoops = 0;
		_accTime = 0;
	}

	return _fps;
}

}