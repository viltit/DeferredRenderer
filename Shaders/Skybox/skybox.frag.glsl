#version 400 core

in vec3 uv;
out vec4 color;

uniform samplerCube skybox;

void main() {
	color = texture(skybox, uv);
}