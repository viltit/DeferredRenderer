#version 400 core

in vec2 fUV;

layout(location = 0) out vec4 fcolor;
layout(location = 1) out vec4 brightColor;

uniform sampler2D color;
uniform sampler2D diffuse;
uniform sampler2D specular;

uniform float treshold = 1.0f;

void main() {
	/* diffuse and specular color coming from the object: */
	vec4 allColor = texture(color, fUV);
	vec3 diffuseColor = allColor.rgb;
	vec3 specularColor = allColor.aaa;

	/* diffuse and specular values coming from the light: */
	vec3 diffuseLight = texture(diffuse, fUV).rgb;
	vec3 specularLight = texture(specular, fUV).rgb;

	/* provisorium: ambient */
	vec3 ambientColor = texture(color, fUV).rgb * 0.2f;

	fcolor = vec4(ambientColor + (diffuseColor * diffuseLight) + (specularColor * specularLight), 1.0f);

	//extract bright colors for bloom:
	float brightness = dot(fcolor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > treshold) { 
		brightColor = vec4(fcolor.rgb, 1.0f);
	}
	else {
		brightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}
}