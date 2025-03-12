#include "Water.h"

#pragma region Wave

Wave::Wave() : amplitude(0.0f), frequency(0.0f), phase(0.0f) {};

Wave::Wave(float wavelength, float amplitude, float speed)
{
	this->amplitude = amplitude;
	this->frequency = 2.0f / wavelength;
	this->phase = speed * frequency;
	// this->direction
}

Wave::~Wave() {};

Water::Water() : planeRes(10), planeLen(10), wavesUboID(0) {};

Water::Water(int planeRes, int planeLen) 
	: planeRes(planeRes), planeLen(planeLen), wavesUboID(0) {};

Water::~Water() {};

#pragma endregion

#pragma region Water

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

void Water::generateWaves()
{
	// Initialize waves
	waves[0] = Wave(4.0f, 1.0f, 1.5f);
	waves[1] = Wave(2.0f, 0.5f, 1.5f);

	// Send the waves to the GPU
	setupWavesUbo();
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