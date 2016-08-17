/* hide some neccessary global variables behind a namespace */

#pragma once

#define CONSOLE_LOG		//give messages about programm progress to the console

namespace vitiGL {

	namespace globals {
		extern int window_w;
		extern int window_h;
	}
	
}