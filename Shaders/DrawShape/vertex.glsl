/*		Shader to draw a blank object with no texture 
		(Original use is to draw the AABB boxes in wireframe mode)
*/

#version 400 core

layout(location = 0) in vec3 pos;

uniform mat4 VP;
uniform mat4 M;

void main() {
	gl_Position = VP * M * vec4(pos, 1.0f);
}