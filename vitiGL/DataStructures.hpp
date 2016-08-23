/*	Structures for some often needed data types, like Colors */

#pragma once

namespace vitiGL {
struct Color {
	Color(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha = 0xFF)
		:	r{ red },
			g{ green },
			b{ blue },
			a{ alpha }
	{}

	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};
}