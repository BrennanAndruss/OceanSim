#pragma once

#ifndef TEXTURE_H
#define	TEXTURE_H

#include <string>

#include <glad/glad.h>

class Texture
{
public:
	Texture();
	~Texture();

	void setUnit(GLint u) { unit = u; }
	GLint getUnit() const { return unit; }

	bool init(const std::string& file, bool alpha);
	void bind(GLint handle);
	void unbind();

	

private:
	int width;
	int height;
	GLint internalFormat;
	GLint imageFormat;

	GLuint tid = 0;
	GLint unit;
};

#endif
