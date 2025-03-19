#pragma once

#ifndef WATER_H
#define WATER_H

#include <vector>
#include "Mesh.h"

#define MAX_WAVES 16


// Aligned for std140 alignment
// Using vec4 for 16-byte alignment
struct alignas(16) Wave
{
	glm::vec4 direction;	// {Dx, Dz, 0, 0}
	float amplitude;
	float frequency;
	float phase;

	Wave();
	Wave(float amplitude, float wavelength, float speed, glm::vec2 direction);
	~Wave();

	glm::vec2 getDirection() const;
};


class Water
{
public:
	Water();
	Water(int planeRes, int planeLen);
	~Water();

	void generateMesh();
	void generateWaves(unsigned int seed);
	void setupWavesUbo();
	void updateWavesUbo();
	void draw() const;

private:
	int planeRes, planeLen;

	Mesh mesh;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texCoords;

	Wave waves[MAX_WAVES];
	GLuint wavesUboID;
};

#endif // WATER_H
