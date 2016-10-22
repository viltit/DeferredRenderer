#version 400 core

uniform vec4 col;

out vec4 color;

void main() {
	color = vec4(col);
}