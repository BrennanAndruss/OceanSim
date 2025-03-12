#version 420 core // For UBO binding support

#define PI 3.1415926538

#define MAX_WAVES 2

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

struct Wave
{
	float amplitude;
	float frequency;
	float phase;
};

layout(std140, binding = 1) uniform Waves
{
	Wave waves[MAX_WAVES];
};

out vec2 TexCoord;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

uniform float time;

void main()
{
	vec3 p = aPos;
	for (int i = 0; i < MAX_WAVES; i++)
	{
		Wave w = waves[i];
		float waveHeight = w.amplitude * sin(w.frequency * p.x + time * w.phase);
		p.y += waveHeight;
	}

	gl_Position = P * V * M * vec4(p, 1.0);
}