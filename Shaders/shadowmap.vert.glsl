/* very simple Shader to create a Shadowmap */

#version 400 core

layout (location = 0) in vec3 pos;

uniform	mat4 T;		/* light-space matrix */
uniform mat4 M;		/* world space matrix */

void main(void) {
	gl_Position = T * M * vec4(pos, 1.0f);
}