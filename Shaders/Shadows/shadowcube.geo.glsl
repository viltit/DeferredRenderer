#version 400 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 L[6];		/* light space matrix */

out vec4 fragPos;

void main(void) {
	for (int i = 0; i < 6; i++) {
		gl_Layer = i;	//to draw on the correct face of the cubemap
		for (int j = 0; j < 3; j++) {
			fragPos = gl_in[j].gl_Position;
			gl_Position = L[i] * fragPos;
			EmitVertex();
		}
		EndPrimitive();
	}
}