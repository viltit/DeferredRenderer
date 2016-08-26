#version 400 core

in vec2 fUV;

out vec4 fcolor;

uniform sampler2D color;
uniform sampler2D diffuse;
uniform sampler2D specular;
uniform sampler2D shadow;

void main() {
	/* diffuse and specular color coming from the object: */
	vec4 allColor = texture(color, fUV);
	vec3 diffuseColor = allColor.rgb;
	vec3 specularColor = allColor.aaa;

	/* diffuse and specular values coming from the light: */
	vec3 diffuseLight = texture(diffuse, fUV).rgb;
	vec3 specularLight = texture(specular, fUV).rgb;

	/* shadow: */
	float s = texture(shadow, fUV).r;

	/* provisorium: ambient */
	vec3 ambientColor = texture(color, fUV).rgb * 0.2f;

	fcolor = vec4(ambientColor + (diffuseColor * diffuseLight * s) + (specularColor * specularLight * s), 1.0f);
}