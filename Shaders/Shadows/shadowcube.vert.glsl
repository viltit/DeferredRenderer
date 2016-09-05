/* very simple Shader to create a Shadowcube */

#version 400 core

layout (location = 0) in vec3 pos;

uniform mat4 M;		/* world space matrix */

void main(void) {
	gl_Position = M * vec4(pos, 1.0f);
}