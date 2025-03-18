#version 420 core // For UBO binding support

layout (location = 0) in vec4 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 fragPos;
out vec3 fragNor;
out vec2 texCoord;

layout (std140, binding = 0) uniform Matrices
{
    mat4 projection;
    mat4 view;
};
uniform mat4 model;

void main()
{
	// gl_Position = P * V * model * aPos;
	gl_Position = projection * view * model * aPos;
	fragPos = vec3(model * aPos);
	fragNor = vec3(model * vec4(aNormal, 1.0));
	texCoord = aPos.xy;
}
