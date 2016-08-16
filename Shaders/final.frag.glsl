#version 400 core

in vec2 fUV;

out vec4 color;

uniform sampler2D image;
uniform sampler2D shadowMap;

void main() {
	float shadow = texture(shadowMap, fUV).r + 0.3;
	color = texture(image, fUV);
	color = vec4(color.r * shadow, color.g * shadow, color.b * shadow, color);
}
