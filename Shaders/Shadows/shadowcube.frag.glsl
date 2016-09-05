#version 400 core

in vec4 fragPos;

uniform vec3 pos;
uniform float radius;

void main() {
	//get the distance between the light source and the fragment:
	float dist = length(fragPos.xyz - pos);

	//transform to range [0, 1]:
	dist = dist / radius;

	gl_FragDepth = dist;
}