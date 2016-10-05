/* 	shader to visualize the normal vectors,
	mainly for debugging purposes right now */

#version 400 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out VS_OUT {
	vec3 normal;
} vs_out;

uniform mat4 M;
uniform mat4 P;
uniform mat4 V;

void main() {
	gl_Position = P * V * M * vec4(position, 1.0f);
	
	/* transform normal into clip space for the geometry shader: */
	mat3 normMat = mat3(transpose(inverse(V * M)));
	
	vs_out.normal = normalize(vec3(P * vec4(normMat * normal, 1.0f)));
}