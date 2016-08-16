#version 400 core

//Directional Light: ---------------------------------------
struct dLight {
	vec3 dir;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

//Point Light
struct pLight {
	vec3 pos;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 attenuation; //components constant, linear and quadratic
};

//Material ------------------------------------------------
struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
	float shiny;
};

//Uniforms: ------------------------------------------------
uniform vec3 viewPos;
uniform Material material;
uniform dLight dlight;
uniform pLight plight;

//Input: ---------------------------------------------------
in VS_OUT {
	vec3 pos;
	vec3 normal;
	vec2 uv;

	vec3 tLightPos;	//tangent-space vectors
	vec3 tLightDir;
	vec3 tViewPos;
	vec3 tFragPos;
} fsIn;

//Output: --------------------------------------------------
out vec4 color;

//Function Prototypes: ------------------------------------
vec3 updateDlight(dLight light, vec3 normal, vec3 viewDir); 
vec3 updatePlight(pLight light, vec3 normal, vec3 viewDir);

float updateDirShadow(int c);

//Shaders entry Point -------------------------------------
void main() {
	vec3 normal = texture(material.normal, fsIn.uv).rgb;
	normal = normalize(normal * 2.0f - 1.0f); //bring from range [0, 1] to [-1, 1]
	vec3 viewDir = normalize(fsIn.tViewPos - fsIn.tFragPos);

	vec3 colorDir = updateDlight(dlight, normal, viewDir);
	vec3 colorPos = updatePlight(plight, normal, viewDir);

	color = vec4(colorDir + colorPos, 1.0f);

	//color = vec4(fsIn.clipZ, fsIn.clipZ, fsIn.clipZ, 1.0f);
}

//dLight ---------------------------------------------------
vec3 updateDlight(dLight light, vec3 normal, vec3 viewDir) {
	vec3 lightDir = normalize(-fsIn.tLightDir);
	vec3 halfway = normalize(viewDir + lightDir);

	float diff = max(dot(normal, lightDir), 0.0f);
	float spec = pow(max(dot(normal, halfway), 0.0f), material.shiny);

	//put all lights together and return:
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, fsIn.uv));
	vec3 diffuse = diff * light.diffuse * vec3(texture(material.diffuse, fsIn.uv));
	vec3 specular = spec * light.specular * vec3(texture(material.specular, fsIn.uv));

	return (ambient + (diffuse + specular));
}

//pLight ------------------------------------------------------
vec3 updatePlight(pLight light, vec3 normal, vec3 viewDir) {
	vec3 lightDir = normalize(fsIn.tLightPos - fsIn.tFragPos);
	vec3 halfway = normalize(viewDir + lightDir);
	
	float d = length(light.pos - fsIn.pos);
	float attenuation = 1.0f / (light.attenuation.x + light.attenuation.y * d + light.attenuation.z * d * d);

	float diff = max(dot(normal, lightDir), 0.0f);
	float spec = pow(max(dot(normal, halfway), 0.0f), material.shiny);

	vec3 ambient = light.ambient * vec3(texture(material.diffuse, fsIn.uv));
	vec3 diffuse = diff * light.diffuse * vec3(texture(material.diffuse, fsIn.uv));
	vec3 specular = spec * light.specular * vec3(texture(material.specular, fsIn.uv));

	return (attenuation * (ambient + diffuse + specular));
}