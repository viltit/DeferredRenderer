/*--------------------------------------------------------------------------
Shader for lightning pass in deferred rendering

->	calculates diffuse and specular values of directional and point lights
->	works with a subroutine uniform so our app does not need to switch between
	Shaders for dir and point lights

ToDo:	
->	!Read the shadowmap in this shader! 
->	Read the shinyness value from the alpha-channel of the specular texture

--------------------------------------------------------------------------- */

#version 400 core

/* light structures: ------------------------------------------------------ */
struct DLight {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 dir;
};

struct PLight {
	vec3	pos;
	vec3	diffuse;
	vec3	specular;
	vec3	attenuation; //x->constant, y->linear, z->quadratic
};

struct Light {
	vec3 ambient;	//TO DO: We don't store ambient values in this shader!
	vec3 diffuse;
	vec3 specular;
};

/* Subroutines to switch between point lights and dir lights via uniform: */
subroutine Light UpdateLight(vec3 worldPos, vec3 worldNorm, vec2 uv);
subroutine uniform UpdateLight updateLight;

/* texture output of this shader: ---------------------------------------- */
layout (location = 0) out vec4 ldiffuse;
layout (location = 1) out vec4 lspecular;

/* texture input from geometry pass: ------------------------------------- */
//uniform sampler2D depht;
uniform sampler2D normal;
uniform sampler2D position;
uniform sampler2D shadowmap;
uniform sampler2D shadowcube;

/* more uniforms we need: ------------------------------------------------ */
uniform vec2 texelSize;
uniform vec3 viewPos;

uniform DLight dlight;
uniform	PLight	plight;

out vec4 color;

/* Function prototypes: -------------------------------------------------- */
vec2 getUV();

/* Shaders entry point: -------------------------------------------------- */
void main() {
	/* get uv coordinates, fragments world position and the normal: */
	vec2 uv = getUV();
	vec3 worldPos	= texture(position, uv).xyz;
	vec3 normal		= normalize(texture(normal, uv).xyz);

	Light light = updateLight(worldPos, normal, uv);

	ldiffuse = vec4(light.diffuse, 1.0f);
	lspecular = vec4(light.specular, 1.0f);
}

/* Functions ------------------------------------------------------------- */
vec2 getUV() {
	return vec2(gl_FragCoord.x * texelSize.x, gl_FragCoord.y * texelSize.y);
}

subroutine (UpdateLight)
Light updateDlight(vec3 worldPos, vec3 normal, vec2 uv) {
	vec3 ldir		= normalize(-dlight.dir);
	vec3 viewDir	= normalize(viewPos - worldPos);
	vec3 halfway	= normalize(viewDir + ldir);

	float diff		= clamp(dot(normal, ldir), 0.0f, 1.0f);
	float spec		= pow(clamp(dot(normal, halfway), 0.0f, 1.0f), 22.0f);	//TO DO: 22.0f needs to be a variable !

	/* shadow: */
	float shadow = texture(shadowmap, uv).r;

	Light light;

	light.ambient	= dlight.ambient;
	light.diffuse	= shadow * diff * dlight.diffuse;
	light.specular	= shadow * spec * dlight.specular;

	return light;
}

subroutine (UpdateLight)
Light updatePlight(vec3 worldPos, vec3 normal, vec2 uv) {
	/* Check the distance from fragment to light source: */
	float dist = length(plight.pos - worldPos);
	float attenuation = 1.0f / (plight.attenuation.x + plight.attenuation.y * dist + plight.attenuation.z * dist * dist);

	/* abort if fragment is outside the lights radius: */
	if (attenuation < 1.0f / 256.0f) discard;	//if-branch in fragment shader is suboptimal!

	vec3 lightDir	= normalize(plight.pos - worldPos);
	vec3 viewDir	= normalize(viewPos - worldPos);
	vec3 halfway	= normalize(lightDir + viewDir);

	float diff		= clamp(dot(normal, lightDir), 0.0f, 1.0f);
	float spec		= pow(clamp(dot(normal, halfway), 0.0f, 1.0f), 22.0f);	//TO DO: 22.0f needs to be a variable !

	/* shadow: */
	float shadow = 1.0f - texture(shadowcube, uv).r / 2.0f;

	Light light;

	light.ambient	= vec3(0.0f, 0.0f, 0.0f);
	light.diffuse	= shadow * attenuation * diff * plight.diffuse;
	light.specular	= shadow * attenuation * spec * plight.specular;

	/* debug: 
	light.ambient = vec3(0.0f, 0.0f, 0.0f);
	light.diffuse = attenuation * vec3(1.0, 0.0f, 0.0f);
	light.specular = attenuation * vec3(1.0f, 0.0f, 0.0f);*/

	return light;
}