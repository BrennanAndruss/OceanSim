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

vec3 sinePartials(vec3 v, Wave w)
{
	vec2 wDir = w.direction.xy;	// {Dx, Dz}
	float xz = dot(v.xz, wDir);

	// Calculate partial derivatives of the sine function
	vec2 partials = w.frequency * w.amplitude * wDir * cos(xz * w.frequency + time * w.phase);
	return vec3(partials.x, 1.0, partials.y);
}

vec3 calculateDisplacement(vec3 v, Wave w)
{
	// Sine waves
	return vec3(0.0, sine(v, w), 0.0);
}

vec3 calculatePartials(vec3 v, Wave w)
{
	// Sine waves
	return sinePartials(v, w);
}

void main()
{
	// H(x, z, t) = sum of sines
	// P(x, z, t) = [x, H(x, z, t), z]
	vec3 p = aPos;
	vec3 partials = vec3(0.0);

	// Sum displacement and partial derivatives of all waves
	for (int i = 0; i < MAX_WAVES; i++)
	{
		p += calculateDisplacement(aPos, waves[i]);
		partials += calculatePartials(aPos, waves[i]);
	}

	// Calculate the normal by crossing the summed binormal and tangent vectors
	// B = [0, pd/pdz{P}, 1]
	// T = [1, pd/pdx{P}, 0]
	// N = B x T = [-pd/pdx{P}, 1, -pd/pdz{P}]
	vec3 n = normalize(vec3(-partials.x, 1.0, -partials.z));

	gl_Position = projection * view * model * vec4(p, 1.0);

	fragPos = vec3(model * vec4(p, 1.0));
	fragNor = normalize(vec3(model * vec4(n, 1.0)));
}