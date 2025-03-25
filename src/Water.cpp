#include "Water.h"

#include <random>
#include <glm/gtc/type_ptr.hpp>


#pragma region Wave

Wave::Wave() : amplitude(1.0f), frequency(1.0f), phase(1.0f), steepness(1.0f),
	direction(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)) {};

Wave::Wave(float amplitude, float wavelength, float speed, 
	float steepness, glm::vec2 direction)
{
	this->amplitude = amplitude;
	this->frequency = glm::two_pi<float>() / wavelength;
	this->phase = speed * frequency;
	this->steepness = steepness;
	direction = glm::normalize(direction);
	this->direction = glm::vec4(direction.x, direction.y, 0.0f, 0.0f);
}

Wave::~Wave() {};

glm::vec2 Wave::getDirection() const
{
	return glm::vec2(direction.x, direction.y);
}

// Generates a flat wave with no effect on the water surface
Wave flatWave()
{
	return Wave(0.0f, 1.0f, 0.0f, 1.0f, glm::vec2(1.0f, 0.0f));
}

#pragma endregion


#pragma region Water

Water::Water() : planeRes(10), planeLen(10), waveFunction(SINE), wavesUboID(0) {};

Water::Water(int planeRes, int planeLen) 
	: planeRes(planeRes), planeLen(planeLen), waveFunction(SINE), wavesUboID(0) {};

Water::~Water() {};

void Water::generateMesh()
{
	float halfLen = planeLen * 0.5f;
	float stepLen = (float)planeLen / planeRes;

	// Initialize normals to up vector
	normals.resize((planeRes + 1) * (planeRes + 1), glm::vec3(0.0f, 1.0f, 0.0f));

	// Initialize vertex attributes
	positions.resize((planeRes + 1) * (planeRes + 1));
	std::vector<glm::vec2> texCoords((planeRes + 1) * (planeRes + 1));
	for (int i = 0, x = 0; x <= planeRes; x++)
	{
		for (int z = 0; z <= planeRes; i++, z++)
		{
			positions[i] = glm::vec3(x * stepLen - halfLen, 0.0f, z * stepLen - halfLen);
			texCoords[i] = glm::vec2((float)x / planeRes, (float)z / planeRes);
		}
	}
	
	// Initialize indices to triangulate the plane
	std::vector<unsigned int> indices(planeRes * planeRes * 6);
	for (int ti = 0, vi = 0, x = 0; x < planeRes; vi++, x++)
	{
		for (int z = 0; z < planeRes; ti += 6, vi++, z++)
		{
			// Set triangle indices in counterclockwise winding order
			indices[ti] = vi;
			indices[ti + 1] = vi + 1;
			indices[ti + 2] = vi + planeRes + 1;
			indices[ti + 3] = vi + planeRes + 1;
			indices[ti + 4] = vi + 1;
			indices[ti + 5] = vi + planeRes + 2;
		}
	}

	// Set the mesh to dynamic, since the vertex data will change frequently
	mesh.setDynamic(true);

	// Send the mesh data to the GPU
	mesh.setupBuffers(positions, normals, texCoords, indices);
}

void Water::generateWaves(unsigned int seed)
{
	std::mt19937 generator(seed);

	// Define distributions for wave parameters
	std::uniform_real_distribution<float> amplitudeDist(0.01f, 0.05f);
	std::uniform_real_distribution<float> wavelengthDist(2.0f, 8.0f);
	std::uniform_real_distribution<float> speedDist(0.5f, 2.0f);
	std::uniform_real_distribution<float> steepnessDist(1.0f, 4.0f);
	std::uniform_real_distribution<float> angleDist(
		-glm::quarter_pi<float>(), glm::quarter_pi<float>());

	float baseAngle = 0.0f;

	for (int i = 0; i < MAX_WAVES; i++)
	{
		// Generate random wave parameters
		float amplitude = amplitudeDist(generator);
		float wavelength = wavelengthDist(generator);
		float speed = speedDist(generator);
		float steepness = steepnessDist(generator);
		
		float angle = baseAngle + angleDist(generator);
		glm::vec2 direction = glm::vec2(sin(angle), -cos(angle));

		waves[i] = Wave(amplitude, wavelength, speed, steepness, direction);
	}

	// Send the waves to the GPU
	setupWavesUbo();
}

float Water::sine(glm::vec3 position, Wave w, float time) const
{
	float xz = glm::dot(glm::vec2(position.x, position.z), w.getDirection());
	return w.amplitude * sin(xz * w.frequency + time * w.phase);
}

float Water::steepSine(glm::vec3 position, Wave w, float time) const
{
	float xz = glm::dot(glm::vec2(position.x, position.z), w.getDirection());
	return 2.0f * w.amplitude * 
		pow((sin(xz * w.frequency + time * w.phase) + 1.0f) / 2.0f, w.steepness);
}

glm::vec3 Water::gerstner(glm::vec3 position, Wave w, float time) const
{
	float xz = glm::dot(glm::vec2(position.x, position.z), w.getDirection());
	float f = xz * w.frequency + time * w.phase;

	float s = sin(f);
	float c = cos(f);

	glm::vec3 g(0.0);
	g.x = w.steepness * w.amplitude * w.getDirection().x * c;
	g.y = w.amplitude * s;
	g.z = w.steepness * w.amplitude * w.getDirection().y * c;

	return g;
}

glm::vec3 Water::getDisplacement(glm::vec3 position, float time) const
{
	if (waveFunction == SINE)
	{
		float displacement = 0.0f;
		for (int i = 0; i < MAX_WAVES; i++)
		{
			displacement += sine(position, waves[i], time);
		}
		return glm::vec3(position.x, displacement, position.z);
	}

	else if (waveFunction == STEEP_SINE)
	{
		float displacement = 0.0f;
		for (int i = 0; i < MAX_WAVES; i++)
		{
			displacement += steepSine(position, waves[i], time);
		}
		return glm::vec3(position.x, displacement, position.z);
	}

	else if (waveFunction == GERSTNER)
	{
		glm::vec3 displacement(0.0f);
		for (int i = 0; i < MAX_WAVES; i++)
		{
			displacement += gerstner(position, waves[i], time);
		}
		return glm::vec3(position.x, displacement.y, position.z);
	}
	else
	{
		return position;
	}
}

void Water::setupWavesUbo()
{
	// Initialize UBO with wave data
	glGenBuffers(1, &wavesUboID);
	glBindBuffer(GL_UNIFORM_BUFFER, wavesUboID);
	glBufferData(GL_UNIFORM_BUFFER, MAX_WAVES * sizeof(Wave), waves, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Bind UBO	to binding point 1
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, wavesUboID);
}

void Water::updateWavesUbo()
{
	glBindBuffer(GL_UNIFORM_BUFFER, wavesUboID);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, MAX_WAVES * sizeof(Wave), waves);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Water::draw() const
{
	mesh.draw();
}

#pragma endregion