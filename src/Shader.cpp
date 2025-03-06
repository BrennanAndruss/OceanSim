#include "Shader.h"

#include <iostream>
#include <fstream>
#include "GLSL.h"

std::string readFileAsString(const std::string& fileName)
{
	std::string result;
	std::ifstream fileHandle(fileName);

	if (fileHandle.is_open())
	{
		fileHandle.seekg(0, std::ios::end);
		result.reserve((size_t)fileHandle.tellg());
		fileHandle.seekg(0, std::ios::beg);

		result.assign((std::istreambuf_iterator<char>(fileHandle)), std::istreambuf_iterator<char>());
	}
	else
	{
		std::cerr << "Could not open file: '" << fileName << "'" << std::endl;
	}

	return result;
}

bool Shader::init(const std::string& vShaderFile, const std::string& fShaderFile)
{
	// Set the shader names
	vShaderName = vShaderFile;
	fShaderName = fShaderFile;

	// Retrieve shader source code from the file
	std::string vertexCode = readFileAsString(vShaderFile);
	std::string fragmentCode = readFileAsString(fShaderFile);
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();


	GLint rc;

	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

	CHECKED_GL_CALL(glShaderSource(vShader, 1, &vShaderCode, NULL));
	CHECKED_GL_CALL(glShaderSource(fShader, 1, &fShaderCode, NULL));

	// Compile vertex shader
	CHECKED_GL_CALL(glCompileShader(vShader));
	CHECKED_GL_CALL(glGetShaderiv(vShader, GL_COMPILE_STATUS, &rc));
	if (!rc)
	{
		if (verbose)
		{
			GLSL::printShaderInfoLog(vShader);
			std::cout << "Error compiling vertex shader " << vShaderName << std::endl;
		}
		return false;
	}

	// Compile fragment shader
	CHECKED_GL_CALL(glCompileShader(fShader));
	CHECKED_GL_CALL(glGetShaderiv(fShader, GL_COMPILE_STATUS, &rc));
	if (!rc)
	{
		if (verbose)
		{
			GLSL::printShaderInfoLog(fShader);
			std::cout << "Error compiling fragment shader " << fShaderName << std::endl;
		}
		return false;
	}

	// Create the program and link
	pid = glCreateProgram();
	CHECKED_GL_CALL(glAttachShader(pid, vShader));
	CHECKED_GL_CALL(glAttachShader(pid, fShader));
	CHECKED_GL_CALL(glLinkProgram(pid));
	CHECKED_GL_CALL(glGetProgramiv(pid, GL_LINK_STATUS, &rc));
	if (!rc)
	{
		if (verbose)
		{
			GLSL::printProgramInfoLog(pid);
			std::cout << "Error linking shaders " << vShaderName << " and " << fShaderName << std::endl;
		}
		return false;
	}

	return true;
}

void Shader::bind()
{
	CHECKED_GL_CALL(glUseProgram(pid));
}

void Shader::unbind()
{
	CHECKED_GL_CALL(glUseProgram(0));
}
