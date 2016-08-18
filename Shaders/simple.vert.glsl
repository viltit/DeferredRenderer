#version 400 core

layout (location = 0) in vec2 v_pos;
layout (location = 1) in vec2 v_uv;

out vec2 uv;

void main() {
	gl_Position = vec4(v_pos.x, v_pos.y, 0.0f, 1.0f);
	uv			= v_uv;
}