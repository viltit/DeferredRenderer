#version 410 core

/* layout qualifiers to put the values in the right color attachment: */
layout(location = 0) out vec3 g_pos;
layout(location = 1) out vec3 g_norm;
layout(location = 2) out vec4 g_albedo;

in VS_OUT{
	vec3	norm;		/* all in world space */
	vec3	tangent;
	vec3	bitangent;
	vec2	uv;
	vec3	pos;
} fs_in;

uniform sampler2D diffuse;
uniform sampler2D specular;
uniform sampler2D normal;

void main() {
	/* tangent-bitangent-normal matrix: */
	mat3 TBN		= mat3(fs_in.tangent, fs_in.bitangent, fs_in.norm);

	g_norm			= texture(normal, fs_in.uv).rgb;		//sample normal
	g_norm			= normalize(g_norm * 2.0f - 1.0f);		//bring it in range [-1 / 1]
	g_norm			= normalize(TBN * g_norm);				//and transform it to world space

	g_albedo.rgb	= texture(diffuse, fs_in.uv).rgb;
	g_albedo.a		= texture(specular, fs_in.uv).r;
	g_pos			= fs_in.pos;
}