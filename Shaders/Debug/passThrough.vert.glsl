#version 400 core

layout(location = 0) in vec4 pos;
layout(location = 1) in vec4 color;

uniform mat4 VP;

out VSOut {
	vec4 color;
} vsOut;

void main() {
	vec4 vp = VP * vec4(pos.xyz, 1.0f);

	gl_Position = vp;

	vsOut.color = color;
}