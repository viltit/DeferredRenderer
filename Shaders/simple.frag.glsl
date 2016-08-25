#version 400 core

in		vec2	uv;
out		vec4	color;

uniform sampler2D tex;

void main() {
	color = texture(tex, uv);
	//float ccolor = texture(tex, uv).a;
	//color = vec4(ccolor, ccolor, ccolor, 1.0f);
}