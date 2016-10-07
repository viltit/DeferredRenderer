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


vec3 sampleOffsets[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);  

//Shaders entry Point -------------------------------------
void main() {
	//constants:
	float bias = 0.05;
	float shadow = 0.0;

	//get the vector from the light pos to the frag pos:
	vec3 lightDir = fsIn.pos - lightPos;
	float currentDepth = length(lightDir);

	//narrow sampling when viewer is close to the shadow:
	float diskRadius = (1.0 + (currentDepth / radius)) / 25.0;

	//take shadow samples:
	for (int i = 0; i < 20; i++) {
		float depth = texture(shadowMap, lightDir + sampleOffsets[i] * diskRadius).r;
		depth *= radius;  //map from [0;1] to [0; radius]
		shadow += (currentDepth - bias > depth) ? 0.0f : 1.0f;
	}

	shadow /= 20.0f;
	color = vec4(1.0f - shadow, 1.0f - shadow, 1.0f - shadow, 1.0f);
}