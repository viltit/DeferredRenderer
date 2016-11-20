#version 400 core

layout(points) in;
layout(triangle_strip, max_vertices = 41) out;

in VSOut {
	vec4 pos;
	vec4 color;
} geoIn[];

out GEOOut{
	vec4 color;
} geoOut;

uniform float aspect;
const int numEdges = 20;
const float PI2 = 6.2831853;

void main() {
	geoOut.color = geoIn[0].color;

	float radius = geoIn[0].pos.w * 2.5f;

	vec4 center = gl_in[0].gl_Position;

	for (int i = 0; i < numEdges; i++) {
		float angle = float(i) / float(numEdges) * PI2;
		float x = cos(angle) * radius;
		float y = sin(angle) * radius;

		/* calculate and emit vertices: */
		gl_Position = center;
		EmitVertex();

		gl_Position = center + vec4(x * aspect, y, 0.0f, 0.0f);
		EmitVertex();
	}

	gl_Position = center + vec4(radius * aspect, 0.0f, 0.0f, 0.0f);
	EmitVertex();
}