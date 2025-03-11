#pragma once

#ifndef WATER_H
#define WATER_H

#include <vector>
#include "Mesh.h"

class Water
{
public:
	Water();
	Water(int planeRes, int planeLen);
	~Water();

	void generateMesh();
	void draw() const;

private:
	int planeRes, planeLen;

	Mesh mesh;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texCoords;
};

#endif // WATER_H