#version 410 core

/* layout qualifiers to put the values in the right color attachment: */
layout(location = 0) out vec3 g_norm;
layout(location = 1) out vec4 g_albedo;

/* matrerial structure: */
struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
};


in VS_OUT{
	vec3	norm;		/* all in world space */
	vec3	tangent;
	vec3	bitangent;
	vec2	uv;
} fs_in;

/* subroutine to differentiate objects with normal texture and objects without: */
subroutine vec3 GetNormal();
subroutine uniform GetNormal getNormal; 

uniform Material material;

void main() {
	/* tangent-bitangent-normal matrix: */
	g_norm			= getNormal();
	g_albedo.rgb	= texture(material.diffuse, fs_in.uv).rgb;
	g_albedo.a		= texture(material.specular, fs_in.uv).r;
}


/* subroutine Functions for getNormal: */
subroutine(GetNormal)
vec3 getNormalFromVertex() {
	return fs_in.norm;
}

subroutine(GetNormal)
vec3 getNormalFromTexture() {
	mat3 TBN		= mat3(fs_in.tangent, fs_in.bitangent, fs_in.norm);
	vec3 normal		= texture(material.normal, fs_in.uv).rgb;		//sample normal
	normal			= normalize(normal * 2.0f - 1.0f);				//bring it in range [-1 / 1]
	normal			= normalize(TBN * normal);						//and transform it to world space

	return normal;
}