#version 400 core

//Uniforms: ------------------------------------------------
uniform vec3 lightPos;
uniform float radius;

uniform samplerCube shadowMap;

//Input: ---------------------------------------------------
in VS_OUT{
	vec3 pos;
	vec3 normal;
	vec2 uv;
} fsIn;

//Output: --------------------------------------------------
out vec4 color;


//Shaders entry Point -------------------------------------
void main() {
	//get the vector from the light pos to the frag pos:
	vec3 lightDir = fsIn.pos - lightPos;

	//use this vector to sample from the cubemap:
	float depth = texture(shadowMap, lightDir).r;

	//bring depth from range [0, 1] back to its world length:
	depth *= radius;

	//compare the depth to the distance fragmentPos - lightPos:
	float current = length(lightDir);

	float bias = 0.005;
	float shadow = (current - bias > depth) ? 0.0f : 1.0f;

	color = vec4(shadow, shadow, shadow, 1.0f);
}