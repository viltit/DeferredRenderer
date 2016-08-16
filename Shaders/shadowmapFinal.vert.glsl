#version 400 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec3 bitangent;
layout(location = 4) in vec2 uv;

const int CASCADES = 4;

out VS_OUT{
vec3 pos;
vec3 normal;
vec2 uv;

vec4 lFragPos[CASCADES];	//fragment-pos in Light-Space

float clipZ;
} vsOut;

uniform mat4 VP;				//view-perspective matrix
uniform mat4 M;					//model matrix
uniform mat4 L[CASCADES];		//dir light-space matrix


void main() {
	gl_Position = VP * M * vec4(pos, 1.0f);

	vsOut.pos = vec3(M * vec4(pos, 1.0f));
	vsOut.uv = uv;
	vsOut.normal = mat3(transpose(inverse(M))) * normal;


	for (int i = 0; i < CASCADES; i++) {
		vsOut.lFragPos[i] = L[i] * vec4(vsOut.pos, 1.0);
	}

	vsOut.clipZ = gl_Position.z;
}