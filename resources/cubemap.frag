#version 420 core

in vec3 texCoords;

out vec4 fragColor;

uniform samplerCube cubemap;


void main()
{
	fragColor = texture(cubemap, texCoords);
	// fragColor = vec4(texCoords, 1.0);
}