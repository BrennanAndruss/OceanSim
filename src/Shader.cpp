#include "Shader.h"

#include <iostream>
#include <fstream>
#include "GLSL.h"

std::string readFileAsString(const std::string& filepath)
{
	std::string result;
	std::ifstream fileHandle(filepath);

	if (fileHandle.is_open())
	{
		fileHandle.seekg(0, std::ios::end);
		result.reserve((size_t)fileHandle.tellg());
		fileHandle.seekg(0, std::ios::beg);

		result.assign((std::istreambuf_iterator<char>(fileHandle)), 
			std::istreambuf_iterator<char>());
	}
	else
	{
		std::cerr << "Could not open file: '" << filepath 
			<< "'" << std::endl;
	}

	return result;
}

bool Shader::init(const std::string& vShaderFilepath, 
	const std::string& fShaderFilepath)
{
	vShaderName = vShaderFilepath;
	fShaderName = fShaderFilepath;

	// Retrieve shader source code from the file
	std::string vertexCode = readFileAsString(vShaderFilepath);
	std::string fragmentCode = readFileAsString(fShaderFilepath);
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

	CHECKED_GL_CALL(glShaderSource(vShader, 1, &vShaderCode, NULL));
	CHECKED_GL_CALL(glShaderSource(fShader, 1, &fShaderCode, NULL));

	// Compile vertex shader
	GLint rc;
	CHECKED_GL_CALL(glCompileShader(vShader));
	CHECKED_GL_CALL(glGetShaderiv(vShader, GL_COMPILE_STATUS, &rc));
	if (!rc)
	{
		if (verbose)
		{
			GLSL::printShaderInfoLog(vShader);
			std::cout << "Error compiling vertex shader " 
				<< vShaderName << std::endl;
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
			std::cout << "Error compiling fragment shader " 
				<< fShaderName << std::endl;
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
			std::cout << "Error linking shaders " << vShaderName 
				<< " and " << fShaderName << std::endl;
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

GLuint Shader::getPid() const
{
	return pid;
}

void Shader::setBool(const std::string& name, int value) const
{
	CHECKED_GL_CALL(glUniform1i(glGetUniformLocation(pid, name.c_str()), value));
}

void Shader::setInt(const std::string& name, int value) const
{
	CHECKED_GL_CALL(glUniform1i(glGetUniformLocation(pid, name.c_str()), value));
}

void Shader::setFloat(const std::string& name, float value) const
{
	CHECKED_GL_CALL(glUniform1f(glGetUniformLocation(pid, name.c_str()), value));
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
	CHECKED_GL_CALL(glUniform2fv(glGetUniformLocation(pid, name.c_str()), 
		1, &value[0]));
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
	CHECKED_GL_CALL(glUniform3fv(glGetUniformLocation(pid, name.c_str()), 
		1, &value[0]));
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
	CHECKED_GL_CALL(glUniform4fv(glGetUniformLocation(pid, name.c_str()), 
		1, &value[0]));
}

void Shader::setMat2(const std::string& name, const glm::mat2& value) const
{
	CHECKED_GL_CALL(glUniformMatrix2fv(glGetUniformLocation(pid, name.c_str()), 
		1, GL_FALSE, &value[0][0]));
}

void Shader::setMat3(const std::string& name, const glm::mat3& value) const
{
	CHECKED_GL_CALL(glUniformMatrix3fv(glGetUniformLocation(pid, name.c_str()), 
		1, GL_FALSE, &value[0][0]));
}

void Shader::setMat4(const std::string& name, const glm::mat4& value) const
{
	CHECKED_GL_CALL(glUniformMatrix4fv(glGetUniformLocation(pid, name.c_str()), 
		1, GL_FALSE, &value[0][0]));
}