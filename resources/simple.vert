#version 420 core

layout (location = 0) in vec4 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 Pos;
out vec2 TexCoord;

layout (std140, binding = 0) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 P;
uniform mat4 V;
uniform mat4 model;

void main()
{
	// gl_Position = P * V * model * aPos;
	gl_Position = projection * view * model * aPos;
	TexCoord = aPos.xy;
}
