#include "Mesh.h"

#include <iostream>
#include "GLSL.h"

Mesh::Mesh() : vaoID(0), vboID(0), eboID(0), 
	vertUsage(GL_STATIC_DRAW), numIndices(0) {};

Mesh::~Mesh() {}

void Mesh::setDynamic(bool isDynamic)
{
	vertUsage = isDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
}

void Mesh::setupBuffers(std::vector<glm::vec3>& positions,
	std::vector<glm::vec3>& normals, std::vector<glm::vec2>& texCoords,
	std::vector<unsigned int>& indices)
{
	// Generate and initialize the vertex array object
	CHECKED_GL_CALL(glGenVertexArrays(1, &vaoID));
	CHECKED_GL_CALL(glBindVertexArray(vaoID));

	// Generate and bind the vertex buffer object
	CHECKED_GL_CALL(glGenBuffers(1, &vboID));
	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vboID));

	// Allocate space for the data in a single buffer
	vertBuf.resize(positions.size() * VERTEX_ATTRIBUTES);

	// Fill the buffer with the vertex data
	for (size_t i = 0; i < positions.size(); i++)
	{
		size_t index = VERTEX_ATTRIBUTES * i;
		vertBuf[index] = positions[i].x;
		vertBuf[index + 1] = positions[i].y;
		vertBuf[index + 2] = positions[i].z;
		vertBuf[index + 3] = normals[i].x;
		vertBuf[index + 4] = normals[i].y;
		vertBuf[index + 5] = normals[i].z;
		if (!texCoords.empty() && i < texCoords.size())
		{
			vertBuf[index + 6] = texCoords[i].x;
			vertBuf[index + 7] = texCoords[i].y;
		}
	}

	// Bind the vertex data to the vertex buffer object
	CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, vertBuf.size() * sizeof(float), 
		vertBuf.data(), vertUsage));

	// Enable position attribute pointer
	size_t stride = VERTEX_ATTRIBUTES * sizeof(float);
	size_t offset = 0;
	CHECKED_GL_CALL(glEnableVertexAttribArray(0));
	CHECKED_GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		stride, (void*)offset));
	offset += 3 * sizeof(float);

	// Enable normal attribute pointer
	CHECKED_GL_CALL(glEnableVertexAttribArray(1));
	CHECKED_GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		stride, (void*)offset));
	offset += 3 * sizeof(float);

	// Enable texture attribute pointer if available
	if (!texCoords.empty())
	{
		CHECKED_GL_CALL(glEnableVertexAttribArray(2));
		CHECKED_GL_CALL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
			stride, (void*)offset));
	}
	
	// Generate and initialize the element buffer object
	numIndices = indices.size();
	CHECKED_GL_CALL(glGenBuffers(1, &eboID));
	CHECKED_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID));
	CHECKED_GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		numIndices * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW));

	// Unbind the vertex array object
	CHECKED_GL_CALL(glBindVertexArray(0));
}

void Mesh::setupBuffers(const tinyobj::shape_t& shape)
{
	// Generate and initialize the vertex array object
	CHECKED_GL_CALL(glGenVertexArrays(1, &vaoID));
	CHECKED_GL_CALL(glBindVertexArray(vaoID));

	// Generate and bind the vertex buffer object
	CHECKED_GL_CALL(glGenBuffers(1, &vboID));
	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vboID));

	// Allocate space for the data in a single buffer
	size_t numVertices = shape.mesh.positions.size() / 3;
	vertBuf.resize(numVertices * VERTEX_ATTRIBUTES);

	// Fill the buffer with the vertex data, looping through each vertex
	for (size_t i = 0; i < numVertices; i++)
	{
		size_t index = VERTEX_ATTRIBUTES * i;
		vertBuf[index] = shape.mesh.positions[3 * i];
		vertBuf[index + 1] = shape.mesh.positions[3 * i + 1];
		vertBuf[index + 2] = shape.mesh.positions[3 * i + 2];
		vertBuf[index + 3] = shape.mesh.normals[3 * i];
		vertBuf[index + 4] = shape.mesh.normals[3 * i + 1];
		vertBuf[index + 5] = shape.mesh.normals[3 * i + 2];
		if (!shape.mesh.texcoords.empty() && i < shape.mesh.texcoords.size() / 2)
		{
			vertBuf[index + 6] = shape.mesh.texcoords[2 * i];
			vertBuf[index + 7] = shape.mesh.texcoords[2 * i + 1];
		}
	}

	// Bind the vertex data to the vertex buffer object
	CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, vertBuf.size() * sizeof(float),
		vertBuf.data(), vertUsage));

	// Enable position attribute pointer
	size_t stride = VERTEX_ATTRIBUTES * sizeof(float);
	size_t offset = 0;
	CHECKED_GL_CALL(glEnableVertexAttribArray(0));
	CHECKED_GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		stride, (void*)0));
	offset += 3 * sizeof(float);

	// Enable normal attribute pointer
	CHECKED_GL_CALL(glEnableVertexAttribArray(1));
	CHECKED_GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		stride, (void*)offset));
	offset += 3 * sizeof(float);

	// Enable texture attribute pointer if available
	if (!shape.mesh.texcoords.empty())
	{
		CHECKED_GL_CALL(glEnableVertexAttribArray(2));
		CHECKED_GL_CALL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
			stride, (void*)offset));
	}

	// Generate and initialize the element buffer object
	numIndices = shape.mesh.indices.size();
	CHECKED_GL_CALL(glGenBuffers(1, &eboID));
	CHECKED_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID));
	CHECKED_GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		numIndices * sizeof(unsigned int),
		shape.mesh.indices.data(), GL_STATIC_DRAW));

	// Unbind the vertex array object
	CHECKED_GL_CALL(glBindVertexArray(0));
}

void Mesh::updateBuffers(std::vector<glm::vec3>& positions, 
	std::vector<glm::vec3>& normals)
{
	// Update the vertex data
	for (size_t i = 0; i < vertBuf.size(); i += VERTEX_ATTRIBUTES)
	{
		vertBuf[i] = positions[i].x;
		vertBuf[i + 1] = positions[i].y;
		vertBuf[i + 2] = positions[i].z;
		vertBuf[i + 3] = normals[i].x;
		vertBuf[i + 4] = normals[i].y;
		vertBuf[i + 5] = normals[i].z;
	}

	// Bind the vertex buffer object and update the buffer data
	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vboID));
	CHECKED_GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, 
		vertBuf.size() * sizeof(float), vertBuf.data()));
}

void Mesh::draw() const
{
	CHECKED_GL_CALL(glBindVertexArray(vaoID));
	CHECKED_GL_CALL(glDrawElements(GL_TRIANGLES, numIndices, 
		GL_UNSIGNED_INT, (const void*)0));
	CHECKED_GL_CALL(glBindVertexArray(0));
}
