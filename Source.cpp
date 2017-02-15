#include "App.hpp"

#include <conio.h>


int main(int argc, char** argv) {
	try {
		App app("Renderer");
		app.run();
	}
	catch (...) {

	}
	char c = _getch();
	return 0;
}