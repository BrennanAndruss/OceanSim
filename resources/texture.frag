#version 420 core

in vec3 fragPos;
in vec3 fragNor;
in vec2 texCoord;

out vec4 fragColor;

uniform vec3 matAmb;
uniform sampler2D matDif;
uniform sampler2D matSpec;
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
	vec3 difTex = texture(matDif, texCoord).rgb;
	float dC = max(dot(normal, light), 0.0);
	vec3 diffuse = difTex * dC;

	// Specular reflection
	vec3 specTex = texture(matSpec, texCoord).rgb;
	vec3 view = normalize(cameraPos - fragPos);
	vec3 halfway = normalize(light + view);
	float sC = max(dot(normal, halfway), 0.0);
	vec3 specular = specTex * pow(sC, matShine);

	vec3 reflection = ambient + diffuse + specular;
	fragColor = vec4(reflection, 1.0);

//	 fragColor = texture(matDif, texCoord);

	// fragColor = vec4(texCoord.s, texCoord.t, 0.0, 1.0);
}