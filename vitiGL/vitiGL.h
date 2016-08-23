#pragma once
#pragma comment(lib, "SDL2.lib")

#include <SDL2/SDL.h>

#include "Error.hpp"
#include "Window.hpp"

namespace vitiGL {
	void start(int msaa = 4);
	void stop();
}