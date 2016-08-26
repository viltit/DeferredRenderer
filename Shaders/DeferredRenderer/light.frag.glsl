#version 400 core

/* light structures: -------------------------------------------------- */
struct DLight {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 dir;
};

struct Light {
	vec3 ambient;	//TO DO: We don't store ambient values in this shader!
	vec3 diffuse;
	vec3 specular;
};

/* texture output of this shader: -------------------------------------- */
layout (location = 0) out vec4 ldiffuse;
layout (location = 1) out vec4 lspecular;

/* texture input from geometry pass: ----------------------------------- */
//uniform sampler2D depht;
uniform sampler2D normal;
uniform sampler2D position;

/* more uniforms we need: ----------------------------------------------- */
uniform vec2 texelSize;
uniform vec3 viewPos;

uniform DLight dlight;

in vec3 lightPos;

out vec4 color;

/* Function prototypes: ------------------------------------------------ */
vec2 getUV();
Light updateDLight(vec3 worldPos, vec3 worldNorm, vec2 uv);

/* Shaders entry point: ------------------------------------------------ */
void main() {
	/* get uv coordinates, fragments world position and the normal: */
	vec2 uv = getUV();
	vec3 worldPos	= texture(position, uv).xyz;
	vec3 normal		= normalize(texture(normal, uv).xyz);

	Light light = updateDLight(worldPos, normal, uv);

	ldiffuse = vec4(light.diffuse, 1.0f);
	lspecular = vec4(light.specular, 1.0f);
}

/* Functions ------------------------------------------------------------- */
vec2 getUV() {
	return vec2(gl_FragCoord.x * texelSize.x, gl_FragCoord.y * texelSize.y);
}

Light updateDLight(vec3 worldPos, vec3 normal, vec2 uv) {
	vec3 ldir		= normalize(-dlight.dir);
	vec3 viewDir	= normalize(viewPos - worldPos);
	vec3 halfway	= normalize(viewDir + ldir);

	float diff		= clamp(dot(normal, ldir), 0.0f, 1.0f);
	float spec		= pow(clamp(dot(normal, halfway), 0.0f, 1.0f), 22.0f);	//TO DO: 22.0f needs to be a variable !

	Light light;

	light.ambient	= dlight.ambient;
	light.diffuse	= diff * dlight.diffuse;
	light.specular	= spec * dlight.specular;

	return light;
}