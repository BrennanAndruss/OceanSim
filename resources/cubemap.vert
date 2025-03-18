#version 420 core // For UBO binding support

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNor;
layout (location = 2) in vec2 aTexCoord;

out vec3 texCoords;

layout (std140, binding = 0) uniform Matrices
{
    mat4 projection;
    mat4 view;
};
uniform mat4 model;


void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    texCoords = aPos;
}