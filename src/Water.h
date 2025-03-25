#pragma once

#ifndef WATER_H
#define WATER_H

#include <vector>
#include "Mesh.h"
#include "Time.h"

#define MAX_WAVES 16


enum WaveFunction
{
	SINE,
	STEEP_SINE,
	GERSTNER,
};

// Aligned for std140 alignment
// Using vec4 for 16-byte alignment
struct alignas(16) Wave
{
	glm::vec4 direction;	// {Dx, Dz, 0, 0}
	float amplitude;
	float frequency;
	float phase;
	float steepness;

	Wave();
	Wave(float amplitude, float wavelength, float speed, 
		float steepness, glm::vec2 direction);
	~Wave();

	glm::vec2 getDirection() const;
};


class Water
{
public:
	Water();
	Water(int planeRes, int planeLen, WaveFunction waveFunction);
	~Water();

	void generateMesh();
	void generateWaves(unsigned int seed, float medianWavelength, 
		float medianAmplitude, float spreadAngle);

	float sine(glm::vec3 position, Wave w, float time) const;
	float steepSine(glm::vec3 position, Wave w, float time) const;
	glm::vec3 gerstner(glm::vec3 position, Wave w, float tiem) const;
	glm::vec3 getDisplacement(glm::vec3 position, float time) const;
	WaveFunction getWaveFunction() const;

	void setupWavesUbo();
	void updateWavesUbo();
	void draw() const;

private:
	int planeRes, planeLen;
	WaveFunction waveFunction;

	Mesh mesh;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texCoords;

	Wave waves[MAX_WAVES];
	GLuint wavesUboID;
};

#endif // WATER_H
