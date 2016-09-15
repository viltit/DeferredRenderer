#version 400 core

layout (location = 0) in vec3 pos;

uniform mat4 M;
uniform mat4 VP;

out vec3 lightPos;
out mat4 inverseVP;

void main() {
	gl_Position = VP * M * vec4(pos, 1.0f);

	/* for reconstruction of the fragments position: */
	inverseVP = inverse(VP);
}