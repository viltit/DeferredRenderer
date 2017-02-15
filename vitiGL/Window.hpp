/* 
	Class to create a Window with SDL2 that has an OpenGL context attached to it
*/


#pragma once

#include <string>
#include <SDL2\SDL.h>
#include <GL\glew.h>

#undef main		//very ugly!

#include "vitiTypes.hpp"

namespace vitiGL {


enum class wStyle {
	none		= 0x00,
	full		= 0x1,
	max			= 0x2,
	noBorder	= 0x4
};

class Window {
public:
	Window(const std::string& name, unsigned int width, unsigned int height,
		wStyle style = wStyle::max, Color col = { 0x00, 0x00, 0x00, 0xFF });
	~Window();

	void	clear() const;		//Clear screen
	void	swap() const;		//Update screen

	void	centerMouse() const;

	/* getters, inline: */
	int		width() const { return w; }
	int		height() const { return h; }

private:
	SDL_Window*		window;
	unsigned int	w;
	unsigned int	h;
};
}