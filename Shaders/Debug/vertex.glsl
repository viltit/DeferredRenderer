#version 400 core

layout(location = 0) in vec4 pos;
layout(location = 1) in vec4 color;

uniform mat4 VP;

out VSOut {
	vec4 pos;
	vec4 color;
} vsOut;

void main() {
	vec4 glPos = VP * vec4(pos.xyz, 1.0f);
	//glPos.w *= 1.001f;
	
	gl_Position = glPos;

	vsOut.pos = vec4(glPos.xyz, pos.w);
	vsOut.color = color;
}