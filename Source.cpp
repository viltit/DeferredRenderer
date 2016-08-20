#include "Frame.hpp"

#include "App.hpp"

#include <conio.h>


int main(int argc, char** argv) {
	try {
		App app("test");
		app.run();
	}
	catch (...) {

	}
/*
	try {
		vitiGL::start();
		Frame frame;
		vitiGL::stop();
	}
	catch (...) {

	}*/

	char c = _getch();
	return 0;
}