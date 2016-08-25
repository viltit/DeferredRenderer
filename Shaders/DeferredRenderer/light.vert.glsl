#version 400 core

layout (location = 0) in vec3 pos;

uniform mat4 M;
uniform mat4 VP;

out vec3 lightPos;

void main() {
	/* only makes sence for point lights: */
	lightPos = vec3(M * vec4(0.0f, 0.0f, 0.0f, 1.0f));
	gl_Position = VP * M * vec4(pos, 1.0f);
}