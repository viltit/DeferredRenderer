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

uniform Material material;

void main() {
	/* tangent-bitangent-normal matrix: */
	mat3 TBN		= mat3(fs_in.tangent, fs_in.bitangent, fs_in.norm);

	g_norm			= texture(material.normal, fs_in.uv).rgb;		//sample normal
	g_norm			= normalize(g_norm * 2.0f - 1.0f);		//bring it in range [-1 / 1]
	g_norm			= normalize(TBN * g_norm);				//and transform it to world space

	g_albedo.rgb	= texture(material.diffuse, fs_in.uv).rgb;
	g_albedo.a		= texture(material.specular, fs_in.uv).r;
}