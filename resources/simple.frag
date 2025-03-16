#version 330 core

in vec3 Pos;
in vec2 TexCoord;

out vec4 fragColor;

void main()
{
	// Debug view
	fragColor = vec4(TexCoord.s, TexCoord.t, 0.0, 1.0);
}
