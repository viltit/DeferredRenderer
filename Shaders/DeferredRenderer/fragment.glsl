#version 400 core

//Material ------------------------------------------------
struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
	sampler2D alpha;
};

uniform Material material;

//Input: ---------------------------------------------------
in VS_OUT {
	vec2 uv;
} fsIn;


//Output: --------------------------------------------------
out vec4 color;


//Shaders entry Point -------------------------------------
void main() {
	float alpha = texture(material.alpha, fsIn.uv).r;

	color = texture(material.diffuse, fsIn.uv);
	color = vec4(color.rgb, alpha);
	//color = vec4(alpha, 0.0f, 0.0f, 1.0f);
}