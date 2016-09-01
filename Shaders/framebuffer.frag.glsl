#version 400 core

in vec2 fUV;
out vec4 color;

uniform sampler2D tex;

uniform mat3 kernel;
uniform float width;
uniform float height;

uniform float gamma = 1.2;

void main() { 
	float offset_x = 1.0f / width;
	float offset_y = 1.0f / height;

	vec2 offsets[9] = vec2[](
		vec2(-offset_x, offset_y),
		vec2(0.0f, offset_y),
		vec2(offset_x, offset_y),
		vec2(-offset_x, 0.0f),
		vec2(0.0f, 0.0f),
		vec2(offset_x, 0.0f),
		vec2(-offset_x, -offset_y),
		vec2(0.0f, -offset_y),
		vec2(offset_x, -offset_y)
	);

	vec3 sampleTex[9];
	for (int i = 0; i < 9; i++) {
		sampleTex[i] = vec3(texture(tex, fUV + offsets[i]));
	}

	vec3 col = vec3(0.0);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++)
			col += sampleTex[i*3 + j] * kernel[j][i];
	}
	color = vec4(col, 1.0);
	
	//gamma adjustment:
	color.rgb = pow(color.rgb, vec3(1.0/gamma));
}