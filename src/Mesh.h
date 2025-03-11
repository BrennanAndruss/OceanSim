#pragma once

#ifndef _SHAPE_H_
#define _SHAPE_H_

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <tiny_obj_loader/tiny_obj_loader.h>

#define VERTEX_ATTRIBUTES 8

class Mesh
{
public:
	Mesh();
	~Mesh();

	void setDynamic(bool isDynamic);
	void setupBuffers(std::vector<glm::vec3>& positions, 
		std::vector<glm::vec3>& normals, std::vector<glm::vec2>& texCoords, 
		std::vector<unsigned int>& indices);
	void setupBuffers(const tinyobj::shape_t& shape);
	void updateBuffers(std::vector<glm::vec3>& positions, 
		std::vector<glm::vec3>& normals);
	void draw() const;

private:
	std::vector<float> vertBuf;
	size_t numIndices;

	GLuint vaoID, vboID, eboID;
	GLenum vertUsage; // GL_STATIC_DRAW, GL_DYNAMIC_DRAW, GL_STREAM_DRAW
};

#endif