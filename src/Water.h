#pragma once

#ifndef WATER_H
#define WATER_H

#include <vector>
#include "Mesh.h"

#define MAX_WAVES 2

struct Wave
{
	float amplitude;
	float frequency;
	float phase;
	// glm::vec2 direction;

	Wave();
	Wave(float wavelength, float amplitude, float speed);
	~Wave();
};

class Water
{
public:
	Water();
	Water(int planeRes, int planeLen);
	~Water();

	void generateMesh();
	void generateWaves();
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
