#pragma once

#ifndef SHADER_H
#define SHADER_H

#include <string>

#include <glad/glad.h>

class Shader
{
public:
	void setVerbose(const bool v) { verbose = v; }
	bool isVerbose() const { return verbose; }

	bool init(const std::string& vShaderFile, const std::string& fShaderFile);
	void bind();
	void unbind();

	std::string vShaderName;
	std::string fShaderName;

protected:
	

private:
	GLuint pid = 0;
	bool verbose = true;
};

#endif
