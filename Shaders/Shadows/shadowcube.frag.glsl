#version 400 core

in vec4 fragPos;

uniform vec3 pos;
uniform float radius;

void main() {
	//get the distance between the light source and the fragment and transform to range [0, 1]:
	float dist = length(fragPos.xyz - pos) / radius;

	gl_FragDepth = dist;
}