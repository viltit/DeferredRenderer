#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

/* Structures and enums for commonly used types like Colors */

namespace vitiGL {

enum class AppState {
	run,
	pause,
	quit
};

enum class Move {
	forward,
	right,
	backward,
	left,
	up,
	down
};

struct Color {
	Color() {}
	Color(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha = 0xFF) 
		: r{ red }, g{ green }, b{ blue }, a{ alpha }
	{}

	GLubyte r;
	GLubyte g;
	GLubyte b;
	GLubyte a;
};

struct Vertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec3 bitangent;
	glm::vec2 uv;
};

}