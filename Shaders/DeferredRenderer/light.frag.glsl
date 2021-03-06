/*--------------------------------------------------------------------------
Shader for lightning pass in deferred rendering

->	calculates diffuse and specular values of directional and point lights
->	works with a subroutine uniform so our app does not need to switch between
	Shaders for dir and point lights

--------------------------------------------------------------------------- */

#version 450 core

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

/*	Subroutines to switch between point lights and dir lights via uniform: 
	(more complicated than an if branch but better for performance)
*/
subroutine Light UpdateLight(vec3 worldPos, vec3 worldNorm, vec2 uv);
subroutine uniform UpdateLight updateLight;

/* Subroutines for getting the shadow (for shadows on and off) */
subroutine float getDShadow(vec2 uv);
subroutine uniform getDShadow getDirShadow;

subroutine float getPShadow(vec2 uv);
subroutine uniform getPShadow getPointShadow;

/* texture output of this shader: ---------------------------------------- */
layout (location = 0) out vec4 ldiffuse;
layout (location = 1) out vec4 lspecular;

/* texture input from geometry pass: ------------------------------------- */
//uniform sampler2D depht;
uniform sampler2D normal;
uniform sampler2D depth;
uniform sampler2D shadowmap;
uniform sampler2D shadowcube;

/* more uniforms we need: ------------------------------------------------ */
uniform vec2 texelSize;
uniform vec3 viewPos;

uniform DLight dlight;
uniform	PLight	plight;

/* shaders input and output variables: ----------------------------------- */
in mat4 inverseVP;
out vec4 color;


/* Shaders entry point: -------------------------------------------------- */
void main() {
	/* get uv coordinates, fragments world position and the normal: */
	vec2 uv			= vec2(gl_FragCoord.x * texelSize.x, gl_FragCoord.y * texelSize.y);
	vec3 normal		= normalize(texture(normal, uv).xyz);

	/* reconstruct fragments world position: */
	vec3 pos = vec3(uv, texture(depth, uv).r);
	vec4 clip = inverseVP * vec4(pos * 2.0f - 1.0f, 1.0f);

	vec3 worldPos = clip.xyz / clip.w;


	Light light = updateLight(worldPos, normal, uv);

	ldiffuse = vec4(light.diffuse, 1.0f);
	lspecular = vec4(light.specular, 1.0f);
}

/* SUBROUTINE: DLIGHT  ------------------------------------------------------- */
subroutine (UpdateLight)
Light updateDlight(vec3 worldPos, vec3 normal, vec2 uv) {
	vec3 ldir		= normalize(-dlight.dir);
	vec3 viewDir	= normalize(viewPos - worldPos);
	vec3 halfway	= normalize(viewDir + ldir);

	float diff		= clamp(dot(normal, ldir), 0.0f, 1.0f);
	float spec		= pow(clamp(dot(normal, halfway), 0.0f, 1.0f), 22.0f);	//TO DO: 22.0f needs to be a variable !

	/* shadow: */
	float shadow = getDirShadow(uv);

	Light light;

	light.ambient	= dlight.ambient;
	light.diffuse	= shadow * diff * dlight.diffuse;
	light.specular	= shadow * spec * dlight.specular;

	return light;
}


/* SUBROUTINE: PLIGHT  -------------------------------------------------------- */
subroutine (UpdateLight)
Light updatePlight(vec3 worldPos, vec3 normal, vec2 uv) {
	/* Check the distance from fragment to light source: */
	float dist = length(plight.pos - worldPos);
	float attenuation = 1.0f / (plight.attenuation.x + plight.attenuation.y * dist + plight.attenuation.z * dist * dist);

	/* abort if fragment is outside the lights radius: */
	//if (attenuation < 1.0f / 256.0f) discard;	//if-branch in fragment shader is suboptimal!

	vec3 lightDir	= normalize(plight.pos - worldPos);
	vec3 viewDir	= normalize(viewPos - worldPos);
	vec3 halfway	= normalize(lightDir + viewDir);

	float diff		= clamp(dot(normal, lightDir), 0.0f, 1.0f);
	float spec		= pow(clamp(dot(normal, halfway), 0.0f, 1.0f), 22.0f);	//TO DO: 22.0f needs to be a variable !

	/* shadow: */
	float shadow = getPointShadow(uv);

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

/* GET SHADOW VALUE -------------------------------------------- */
subroutine (getDShadow)
float getDShadowOn(vec2 uv) {
	return 1.0f - texture(shadowmap, uv).r;;
}

subroutine (getDShadow)
float getDShadowOff(vec2 uv) {
	return 1.0f;
}

subroutine (getPShadow)
float getPShadowOn(vec2 uv) {
	return 1.0f - texture(shadowcube, uv).r / 2.0f;
}

subroutine (getPShadow)
float getPShadowOff(vec2 uv) {
	return 1.0f;
}