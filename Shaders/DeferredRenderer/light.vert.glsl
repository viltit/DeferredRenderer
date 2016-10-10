#version 400 core

layout (location = 0) in vec3 pos;

uniform mat4 M;
uniform mat4 VP;
uniform mat4 VPl;

out mat4 inverseVP;

void main() {
	gl_Position = VPl * M * vec4(pos, 1.0f);

	/* for reconstruction of the fragments position: */
	inverseVP = inverse(VP);
}