#version 420 core // For UBO binding support

#define PI 3.1415926538

#define MAX_WAVES 16

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNor;
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

vec3 sineNormal(vec3 v, Wave w)
{
	vec2 wDir = w.direction.xy;	// {Dx, Dz}
	float xz = dot(v.xz, wDir);

	// Calculate partial derivatives of the sine function
	vec2 partials = w.frequency * w.amplitude * wDir * cos(xz * w.frequency + time * w.phase);

	// Calculate the normal using the cross product of the partial derivatives
	return vec3(-partials.x, 1.0, -partials.y);
}

vec3 calculateDisplacement(vec3 v, Wave w)
{
	// Sine waves
	return vec3(0.0, sine(v, w), 0.0);
}

vec3 calculateNormal(vec3 v, Wave w)
{
	// Sine waves
	return sineNormal(v, w);
}

void main()
{
	vec3 p = aPos;
	vec3 tangent = vec3(1.0, 0.0, 0.0);
	vec3 binormal = vec3(0.0, 0.0, 1.0);

	// Sum displacement and partial derivatives of all waves
	for (int i = 0; i < MAX_WAVES; i++)
	{
		p += calculateDisplacement(aPos, waves[i]);
		
		vec3 waveNor = calculateNormal(aPos, waves[i]);
		tangent.y += waveNor.x;
		binormal.y += waveNor.z;
	}

	// Compute the final normal by crossing the binormal and tangent vectors
	vec3 n = normalize(cross(binormal, tangent));

	gl_Position = projection * view * model * vec4(p, 1.0);

	fragPos = vec3(model * vec4(p, 1.0));
	fragNor = normalize(vec3(model * vec4(n, 1.0)));
}