#include "vitiGL.hpp"

namespace vitiGL {

	void start(int msaa) {
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			throw initError("Could not initialize SDL. Error-Message: ", SDL_GetError());
		}
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	}

	void stop() {
		SDL_Quit();
	}
}