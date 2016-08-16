#version 400 core

in		vec2	uv;

out		vec4	color;

uniform sampler2D depth;

void main() {
	float value = texture(depth, uv).r;
	color = vec4(vec3(value), 1.0f);
	//color = vec4(uv.x, uv.y, 0.0f, 1.0f);
}