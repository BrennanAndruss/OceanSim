#version 420 core

in vec3 fragPos;
in vec3 fragNor;
in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D Texture0;

void main()
{
	fragColor = texture(Texture0, texCoord);

	// fragColor = vec4(texCoord.s, texCoord.t, 0.0, 1.0);
}