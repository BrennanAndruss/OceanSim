#pragma once
#ifndef _SHAPE_H_
#define _SHAPE_H_

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <tiny_obj_loader/tiny_obj_loader.h>

// Vertex structure for defining the size and layout of data 
// in the vertex buffer object
struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
};

class Mesh
{
public:
	Mesh(bool hasTexCoords, bool isDynamic);
	~Mesh();

	void draw() const;
	void setupBuffers(std::vector<glm::vec3>& positions, 
		std::vector<glm::vec3>& normals, std::vector<glm::vec2>& texCoords, 
		std::vector<unsigned int>& indices);
	void updateBuffers(std::vector<glm::vec3>& positions, std::vector<glm::vec3>& normals);

private:
	std::vector<Vertex> vertBuf;
	GLuint vaoID, vboID, eboID;

	bool hasTexCoords;
	GLenum vertUsage; // GL_STATIC_DRAW, GL_DYNAMIC_DRAW, GL_STREAM_DRAW
};

#endif