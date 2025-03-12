#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

uniform float time;

void main()
{
	vec3 vertPos = aPos;
	vertPos.y = sin(vertPos.x + time);
	gl_Position = P * V * M * vec4(vertPos, 1.0);
}