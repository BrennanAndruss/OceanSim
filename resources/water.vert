#version 420 core // For UBO binding support

#define PI 3.1415926538

#define MAX_WAVES 2

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;

layout (std140, binding = 0) uniform Matrices
{
    mat4 projection;
    mat4 view;
};
uniform mat4 model;

struct Wave
{
	vec4 direction;		// {Dx, Dz, 0, 0}
	float amplitude;
	float frequency;
	float phase;
};

layout(std140, binding = 1) uniform Waves
{
	Wave waves[MAX_WAVES];
};

uniform float time;


float sine(vec3 v, Wave w)
{
	float xz = dot(v.xz, w.direction.xy);
	return w.amplitude * sin(xz * w.frequency + time * w.phase);
}

vec3 calculateDisplacement(vec3 v, Wave w)
{
	// Sine waves
	return vec3(0.0, sine(v, w), 0.0);
}

void main()
{
	vec3 p = aPos;
	for (int i = 0; i < MAX_WAVES; i++)
	{
		p += calculateDisplacement(aPos, waves[i]);
	}
	gl_Position = projection * view * model * vec4(p, 1.0);
}