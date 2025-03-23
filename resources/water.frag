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

uniform samplerCube cubemap;

// Debug flags
uniform bool debugNormals;


void main()
{
	if (debugNormals) {
		fragColor = vec4(normalize(fragNor) * 0.5 + 0.5, 1.0);
		return;
	}

	vec3 normal = normalize(fragNor);
	vec3 light = normalize(-lightDir);
	vec3 view = normalize(cameraPos - fragPos);

	// Ambient lighting
	vec3 ambient = matAmb;

	// Diffuse reflection
	float diffuseFactor = max(dot(normal, light), 0.0);
	vec3 diffuse = matDif * diffuseFactor;

	// Fresnel term with Schlick Approximation
	float baseReflectance = 0.08;
	float viewDot = clamp(dot(view, normal), 0.0, 1.0);
	float fresnelFactor = baseReflectance + (1 - baseReflectance) * 
		pow(1.0 - max(dot(view, normal), 0.0), 5.0);
	
	// Specular reflection with fresnel
	vec3 halfVec = normalize(light + view);

	float specularFactor = pow(max(dot(normal, halfVec), 0.0), matShine);
	vec3 specular = matSpec * specularFactor * fresnelFactor;

	// Cubemap reflection with fresnel
	vec3 reflectionVec = reflect(-view, normal);

	vec3 reflectionColor = texture(cubemap, reflectionVec).rgb;
	vec3 reflection = reflectionColor * fresnelFactor;

	vec3 outColor = ambient + diffuse + specular + reflection;
	fragColor = vec4(outColor, 1.0);
}