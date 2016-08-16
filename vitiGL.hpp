#pragma once

#include <SDL2\SDL.h>
#include <iostream>
#include "Window.hpp"
#include "Camera.hpp"
#include "Error.hpp"
#include "Shape.hpp"
#include "Light.hpp"
#include "Framebuffer.hpp"
#include "Scene.hpp"
#include "Timer.hpp"
#include "Frustum.hpp"
#include "Shadowmap.hpp"
#include "GUI.hpp"

#define CONSOLE_LOG		//give messages about programm progress to the console


namespace vitiGL {

	void start(int msaa = 4);
	void stop();
}
