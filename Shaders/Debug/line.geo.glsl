#version 400 core

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

uniform float aspect;

in VSOut {
	vec4 pos;
	vec4 color;
} geoIn[];

out GEOOut{
	vec4 color;
} geoOut;

void main() {
	geoOut.color = geoIn[0].color;
	
	float size = geoIn[0].pos.w * 1.1f;

	vec4 p0 = gl_in[0].gl_Position;
	vec4 p1 = gl_in[1].gl_Position;

	vec2 ray = p1.xy / abs(p1.w) - p0.xy / abs(p0.w);
	vec2 normal = normalize(vec2(-ray.y, ray.x));

	vec4 offset = vec4(normal.x * aspect, normal.y, 0.0f, 0.0f) * size;

	gl_Position = p0 + offset;
	EmitVertex();
	gl_Position = p0 - offset;
	EmitVertex();
	gl_Position = p1 + offset;
	EmitVertex();
	gl_Position = p1 - offset;
	EmitVertex();
}