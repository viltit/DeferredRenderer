#version 400 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 tangent;
layout (location = 3) in vec3 bitangent;
layout (location = 4) in vec2 uv; 

/* structures for lights: */
struct DLight {
	vec3 dir;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

//Point Light
struct PLight {
	vec3 pos;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 attenuation; //components constant, linear and quadratic
};

out VS_OUT {
	vec3 pos;
	vec3 normal;
	vec2 uv;

	vec3 tViewPos;
	vec3 tFragPos;

	PLight plight;
	DLight dlight;
} vsOut;

uniform mat4 VP;				//view-perspective matrix
uniform mat4 M;					//model matrix

uniform PLight plight;
uniform DLight dlight;

uniform vec3 viewPos;

void main() {
	gl_Position		= VP * M * vec4(pos, 1.0f);

	//calculate the Tangent-Bitangent-Normal-Matrix:
	vec3 T = normalize(vec3(M * vec4(tangent, 0.0f)));
	vec3 B = normalize(vec3(M * vec4(bitangent, 0.0f)));
	vec3 N = normalize(vec3(M * vec4(normal, 0.0f)));

	//to add here: re-orthogonalize with gramm-schmidt-process

	mat3 TBN = transpose(mat3(T, B, N));	//transforms vectors from world to tangent space

	//shaders output:
	vsOut.pos = vec3(M * vec4(pos, 1.0f));
	vsOut.uv = uv;
	vsOut.normal = mat3(transpose(inverse(M))) * normal;

	vsOut.tViewPos = TBN * viewPos;
	vsOut.tFragPos = TBN * vec3(M * vec4(pos, 1.0f));

	//Lights:
	vsOut.plight = plight;
	vsOut.dlight = dlight;
	vsOut.plight.pos = TBN * plight.pos;
	vsOut.dlight.dir = TBN * dlight.dir;
}