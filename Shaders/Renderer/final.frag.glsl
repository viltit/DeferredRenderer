#version 400 core

in vec2 fUV;

out vec4 color;

uniform sampler2D tex;
uniform sampler2D dshadow;
uniform sampler2D pshadow;

void main() {
	color = texture(tex, fUV);

	float shadowP = 1.0f - texture(pshadow, fUV).r * 0.5f;
	float shadowD = 1.0f - texture(dshadow, fUV).r * 0.5f;

	color.rgb = color.rgb * shadowD * shadowP;
}
