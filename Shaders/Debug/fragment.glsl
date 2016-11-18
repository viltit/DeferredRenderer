#version 400 core

in GEOOut{
	vec4 color;
} fsIn;

out vec4 gl_FragColor;

void main() {
	gl_FragColor = fsIn.color;
}