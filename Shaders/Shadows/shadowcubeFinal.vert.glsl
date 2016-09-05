#version 400 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec3 bitangent;
layout(location = 4) in vec2 uv;


out VS_OUT{
	vec3 pos;
	vec3 normal;
	vec2 uv;
} vsOut;

uniform mat4 VP;				//view-perspective matrix
uniform mat4 M;					//model matrix


void main() {
	gl_Position = VP * M * vec4(pos, 1.0f);

	vsOut.pos = vec3(M * vec4(pos, 1.0f));
	vsOut.uv = uv;
	vsOut.normal = mat3(transpose(inverse(M))) * normal;
}