#version 400 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
	vec3 normal;
} gs_in[];

const float SCALE = 0.2f;

void makeLine(int index) {
	gl_Position = gl_in[index].gl_Position;
	EmitVertex();
	gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0f) * SCALE;
	EmitVertex();
	EndPrimitive();
}

void main() {
	makeLine(0);
	makeLine(1);
	makeLine(2);
}