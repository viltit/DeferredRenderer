#version 400 core

in		vec2	uv;
out		vec4	color;

uniform sampler2D tex;

void main() {
	color = texture(tex, uv);
	//color = vec4(1.0f, 1.0f, 0.0f, 1.0f);
}