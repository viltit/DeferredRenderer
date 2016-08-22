#version 400 core

const int CASCADES = 4;

//Uniforms: ------------------------------------------------
uniform vec3 dlightDir;


uniform sampler2DShadow shadowMap[CASCADES];
uniform float cascadeEnd[CASCADES];

//Input: ---------------------------------------------------
in VS_OUT{
vec3 pos;
vec3 normal;
vec2 uv;

vec4 lFragPos[CASCADES];	//fragment-pos in Light-Space

float clipZ;
} fsIn;

//Output: --------------------------------------------------
out vec4 color;

//Function Prototypes: ------------------------------------
float updateDirShadow(int c);

//Shaders entry Point -------------------------------------
void main() {
	float shadow = 1.0f;

	for (int i = 0; i < CASCADES; i++) {
		if (fsIn.clipZ <= cascadeEnd[i]) {
			shadow = updateDirShadow(i);
			break;
		}
	}

	color = vec4(shadow, shadow, shadow, 1.0f);
}

float updateDirShadow(int c) {
	/* do not shadow the backside of an object: */
	if (dot(-dlightDir, fsIn.normal) < 0) return 0;
	
	/* bring fragments light-space coordinate from range [-1 / 1] to [ 0 / 1]: */
	vec4 fragPosNormed = fsIn.lFragPos[c] / fsIn.lFragPos[c].w;
	fragPosNormed = fragPosNormed * 0.5f + 0.5f;

	/* be aware: high bias values may lead to vanishing shadows !!*/
	float bias = max(0.0005 * dot(1.0 - normalize(fsIn.normal), dlightDir), 0.00005);
	fragPosNormed.z -= bias;

	/* take samples and average them for smooth shadows: */
	float shadow = 0;
	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {
			shadow += textureProjOffset(shadowMap[c], fragPosNormed, ivec2(x, y));
		}
	}

	return (shadow / 9.0f);
}