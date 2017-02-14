#include "Window.hpp"
#include "Error.hpp"
#include "vitiGlobals.hpp"

#include <iostream>

namespace vitiGL {

Window::Window(const std::string & name, unsigned int width, unsigned int height, wStyle style, Color col) 
	:	window	{ nullptr },
		w		{ width },
		h		{ height }
{

#ifdef CONSOLE_LOG
	std::cout << "Setting up an OpenGL Window...";
#endif

	//Check for Window style flags with bitwise operators:
	Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_INPUT_GRABBED;
	if (unsigned int(wStyle::noBorder) & unsigned int(style))
		flags |= SDL_WINDOW_BORDERLESS;
	if (unsigned int(wStyle::full) & unsigned int(style))
		flags |= SDL_WINDOW_FULLSCREEN;
	if (unsigned int(wStyle::max) & unsigned int(style))
		flags |= SDL_WINDOW_MAXIMIZED;

	//in case of fullscreen, adjust width and height:
	if ((unsigned int(wStyle::full) & unsigned int(style)) || (unsigned int(wStyle::max) & unsigned int(style))) {
		SDL_DisplayMode resolution;
		SDL_GetDisplayMode(0, 0, &resolution);
		w = resolution.w;
		h = resolution.h;
	}

	//make sure we have a valid with and height:
	if (w <= 0) w = 600;
	if (h <= 0) h = 600;

	//give window width and height to globals namespace:
	globals::window_h = h;
	globals::window_w = w;

	//create the Window:
	window = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, flags);
	if (window == nullptr)
		throw initError("Failed to create a window.", SDL_GetError());

	//create the OpenGL Context:
	if (SDL_GL_CreateContext(window) == nullptr)
		throw initError("Failed to create an OpenGL context.", SDL_GetError());

	//initialize glew:
	if (glewInit() != GLEW_OK)
		throw initError("Failed to initialize Glew.", "");

	//set background color and viewport:
	
	glClearColor(col.r, col.g, col.b, col.a);
	glViewport(0, 0, w, h);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

#ifdef CONSOLE_LOG
	std::cout << "\t\tfinished.\n";
#endif
}

Window::~Window() {
	SDL_DestroyWindow(window);
}

void Window::clear() const {
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::swap() const {
	SDL_GL_SwapWindow(window);
}

}