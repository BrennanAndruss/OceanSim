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
	float steepness;
};

layout(std140, binding = 1) uniform Waves
{
	Wave waves[MAX_WAVES];
};

uniform int waveFunction;
const int SINE = 0;
const int STEEP_SINE = 1;
const int GERSTNER = 2;

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

void sumSines(in vec3 v, out vec3 p, out vec3 n)
{
	// H(x, z, t) = sum of sines
	// P(x, z, t) = [x, H(x, z, t), z]
	p = v;
	vec3 partials = vec3(0.0);

	// Sum displacement and partial derivatives of all waves
	for (int i = 0; i < MAX_WAVES; i++)
	{
		p.y += sine(v, waves[i]);
		partials += sinePartials(v, waves[i]);
	}

	// Calculate the normal by crossing the summed binormal and tangent vectors
	// B = [0, pd/pdz{P}, 1]
	// T = [1, pd/pdx{P}, 0]
	// N = B x T = [-pd/pdx{P}, 1, -pd/pdz{P}]
	n = normalize(vec3(-partials.x, 1.0, -partials.z));
}

float steepSine(vec3 v, Wave w)
{
	float xz = dot(v.xz, w.direction.xy);
	return 2 * w.amplitude * pow((sin(xz * w.frequency + time * w.phase) + 1.0) / 2.0, w.steepness);
}

vec3 steepSinePartials(vec3 v, Wave w)
{
	vec2 wDir = w.direction.xy;	// {Dx, Dz}
	float xz = dot(v.xz, wDir);
	float f = xz * w.frequency + time * w.phase;

	// Calculate partial derivatives of the steep sine function
	float powTerm = pow((sin(f) + 1.0) / 2.0, w.steepness - 1.0);
	float scaleFactor = w.steepness * w.frequency * w.amplitude * powTerm;
	
	vec2 partials = scaleFactor * wDir * cos(f);
	return vec3(partials.x, 1.0, partials.y);
}

void sumSteepSine(in vec3 v, out vec3 p, out vec3 n)
{
	// H(x, z, t) = sum of sines
	// P(x, z, t) = [x, H(x, z, t), z]
	p = v;
	vec3 partials = vec3(0.0);

	// Sum displacement and partial derivatives of all waves
	for (int i = 0; i < MAX_WAVES; i++)
	{
		p.y += steepSine(v, waves[i]);
		partials += steepSinePartials(v, waves[i]);
	}

	// Calculate the normal by crossing the summed binormal and tangent vectors
	// B = [0, pd/pdz{P}, 1]
	// T = [1, pd/pdx{P}, 0]
	// N = B x T = [-pd/pdx{P}, 1, -pd/pdz{P}]
	n = normalize(vec3(-partials.x, 1.0, -partials.z));
}

vec3 gerstner(vec3 v, Wave w)
{
	float xz = dot(v.xz, w.direction.xy);
	float f = xz * w.frequency + time * w.phase;
	
	float s = sin(f);
	float c = cos(f);
	
	vec3 g = vec3(0.0);
	g.x = w.steepness * w.amplitude * w.direction.x * c;
	g.y = w.amplitude * s;
	g.z = w.steepness * w.amplitude * w.direction.y * c;
	
	return g;
}

void gerstnerPartials(vec3 v, Wave w, inout vec3 tangent, inout vec3 binormal)
{
	vec2 wDir = w.direction.xy;	// {Dx, Dz}
	float xz = dot(v.xz, wDir);
	float f = xz * w.frequency + time * w.phase;
	
	float wa = w.frequency * w.amplitude;
	float s = sin(f);
	float c = cos(f);

	tangent += vec3(
		-w.steepness * wDir.x * wDir.x * wa * s,
		wDir.x * wa * c,
		-w.steepness * wDir.x * wDir.y * wa * s
	);

	binormal += vec3(
		-w.steepness * wDir.y * wDir.x * wa * s,
		wDir.y * wa * c,
		-w.steepness * wDir.y * wDir.y * wa * s
	);
}

void sumGerstner(in vec3 v, out vec3 p, out vec3 n)
{
	p = v;
	
	// Sum displacements of all waves to get new positions
	for (int i = 0; i < MAX_WAVES; i++)
	{
		p += gerstner(v, waves[i]);
	}

	vec3 tangents = vec3(1.0, 0.0, 0.0);
	vec3 binormals = vec3(0.0, 0.0, 1.0);

	// Sum partial derivatives of all waves
	for (int i = 0; i < MAX_WAVES; i++)
	{
		// Pass tangent and binormal vectors to accumulate partials
		gerstnerPartials(v, waves[i], tangents, binormals);
	}

	// Calculate the normal by crossing the summed binormal and tangent vectors
	n = normalize(cross(binormals, tangents));
}

void main()
{
	vec3 p, n;

	// Sum all waves to set vertex position and normal
	if (waveFunction == SINE)
	{
		sumSines(aPos, p, n);
	}
	else if (waveFunction == STEEP_SINE)
	{
		sumSteepSine(aPos, p, n);
	}
	else if (waveFunction == GERSTNER)
	{
		sumGerstner(aPos, p, n);
	}
	else
	{
		p = aPos;
		n = aNor;
	}

	gl_Position = projection * view * model * vec4(p, 1.0);

	fragPos = vec3(model * vec4(p, 1.0));
	fragNor = normalize(vec3(model * vec4(n, 1.0)));
}
