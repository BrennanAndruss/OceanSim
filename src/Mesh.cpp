#include "Mesh.h"


#include "GLSL.h"

Mesh::Mesh(bool hasTexCoords, bool isDynamic) 
	: vaoID(0), vboID(0), eboID(0), hasTexCoords(hasTexCoords)
{
	vertUsage = isDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
}

Mesh::~Mesh() {}

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
	vertBuf.resize(positions.size());

	for (size_t i = 0; i < vertBuf.size(); i++)
	{
		vertBuf[i].position = positions[i];
		vertBuf[i].normal = normals[i];
		if (hasTexCoords)
			vertBuf[i].texCoord = texCoords[i];
	}

	// Bind the vertex data to the vertex buffer object
	CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, 
		vertBuf.size() * sizeof(Vertex), vertBuf.data(), vertUsage));

	// Enable position attribute pointer
	size_t offset = 0;
	CHECKED_GL_CALL(glEnableVertexAttribArray(0));	// vertPos
	CHECKED_GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		sizeof(Vertex), (void*)offset));
	offset += sizeof(glm::vec3);

	// Enable normal attribute pointer
	CHECKED_GL_CALL(glEnableVertexAttribArray(1));	// vertNor
	CHECKED_GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		sizeof(Vertex), (void*)offset));
	offset += sizeof(glm::vec3);

	// Enable texture attribute pointer if available
	if (hasTexCoords)
	{
		CHECKED_GL_CALL(glEnableVertexAttribArray(2));	// vertTex
		CHECKED_GL_CALL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
			sizeof(Vertex), (void*)offset));
	}
	
	// Generate and initialize the element buffer object
	CHECKED_GL_CALL(glGenBuffers(1, &eboID));
	CHECKED_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID));
	CHECKED_GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW));

	// Unbind the vertex array object
	CHECKED_GL_CALL(glBindVertexArray(0));
}

void Mesh::updateBuffers(std::vector<glm::vec3>& positions, 
	std::vector<glm::vec3>& normals)
{
	// Update the vertex data
	for (size_t i = 0; i < vertBuf.size(); i++)
	{
		vertBuf[i].position = positions[i];
		vertBuf[i].normal = normals[i];
	}

	// Bind the vertex buffer object and update the buffer data
	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vboID));
	CHECKED_GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, 
		vertBuf.size() * sizeof(Vertex), vertBuf.data()));
}

void Mesh::draw() const
{
	CHECKED_GL_CALL(glBindVertexArray(vaoID));
	CHECKED_GL_CALL(glDrawElements(GL_TRIANGLES, vertBuf.size(), GL_UNSIGNED_INT, 0));
	CHECKED_GL_CALL(glBindVertexArray(0));
}
