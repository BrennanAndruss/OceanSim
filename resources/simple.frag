#version 420 core

in vec3 fragPos;
in vec3 fragNor;
in vec2 texCoord;

out vec4 fragColor;

uniform vec3 matAmb;
uniform vec3 matDif;
uniform vec3 matSpec;
uniform float matShine;

// Directional light
uniform vec3 lightDir;
uniform vec3 cameraPos;

void main()
{
	vec3 normal = normalize(fragNor);
	vec3 light = normalize(-lightDir);

	// Ambient lighting
	vec3 ambient = matAmb;

	// Diffuse reflection
	float dC = max(dot(normal, light), 0.0);
	vec3 diffuse = matDif * dC;
	
	// Specular reflection
	vec3 view = normalize(cameraPos - fragPos);
	vec3 halfway = normalize(light + view);
	float sC = max(dot(normal, halfway), 0.0);
	vec3 specular = matSpec * pow(sC, matShine);

	// Debug overrides
//	ambient = vec3(0.0);
//	specular = vec3(0.0);

	vec3 reflection = ambient + diffuse + specular;
	fragColor = vec4(reflection, 1.0);

	// Debug view
	// fragColor = vec4(fragNor * 0.5 + 0.5, 1.0);
}
