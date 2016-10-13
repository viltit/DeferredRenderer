#version 400 core

layout(location = 0) in vec3 pos;

out vec3 uv;

uniform mat4 V;
uniform mat4 P;

void main() {
	vec4 position = P * V * vec4(pos, 1.0f);
	gl_Position = position.xyww; //so the depth is always maxxed out
	uv = pos;
}