#include "App.hpp"

#include <conio.h>


int main(int argc, char** argv) {
	try {
		App app("test", 600, 600, vitiGL::wStyle::full);
		app.run();
	}
	catch (...) {

	}
	char c = _getch();
	return 0;
}