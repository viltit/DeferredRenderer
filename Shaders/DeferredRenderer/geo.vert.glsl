
#version 410 core


/* ---------------------------- SHADERS INPUT AND OUTPUT ---------------------------- */

layout(location = 0)	in vec3	pos;
layout(location = 1)	in vec3	norm;
layout(location = 2)	in vec3 tangent;
layout(location = 3)	in vec3 bitangent;
layout(location = 4)	in vec2	uv;
//layout(location = 5)	in mat4 I;		/* position matrix for instanced drawing */

out VS_OUT{	
	vec3	norm;		/* all in world space */
	vec3	tangent;
	vec3	bitangent;
	vec2	uv;
} vs_out;

uniform mat4 M;		/* model matrix */
uniform mat4 VP;	/* view-perspective matrix */


/* ---------------------------- SHADERS ENTRY POINT ---------------------------- */

void main() {
	vec3 world_pos		= (M * vec4(pos, 1.0f)).xyz;
	vs_out.uv			= uv;

	/* convert our data into world coordinates: */
	vs_out.norm		= mat3(transpose(inverse(M))) * norm;
	vs_out.tangent	= normalize(vec3(M * vec4(tangent, 0.0f)));
	vs_out.bitangent = normalize(vec3(M * vec4(bitangent, 0.0f)));

	/* Gram-Schmidt process: re-orthogonalize */
	vs_out.tangent		= normalize(vs_out.tangent - dot(vs_out.tangent,vs_out.norm) * vs_out.norm);
	vs_out.bitangent	= normalize(cross(vs_out.tangent, vs_out.norm)); 

	gl_Position			= VP * vec4(world_pos, 1.0f);
}
