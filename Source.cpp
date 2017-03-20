#include "App.hpp"


int main(int argc, char** argv) {
	try {
		App app("Renderer");
		app.run();
	}
	catch (...) {

	}
	return 0;
}
