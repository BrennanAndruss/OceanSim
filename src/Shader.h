#pragma once

#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>

class Shader
{
public:
	void setVerbose(const bool v) { verbose = v; }
	bool isVerbose() const { return verbose; }

	bool init(const std::string& vShaderFile, const std::string& fShaderFile);
	void bind();
	void unbind();

	GLuint getPid() const;

	void setBool(const std::string& name, int value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVec2(const std::string& name, const glm::vec2& value) const;
	void setVec3(const std::string& name, const glm::vec3& value) const;
	void setVec4(const std::string& name, const glm::vec4& value) const;
	void setMat2(const std::string& name, const glm::mat2& value) const;
	void setMat3(const std::string& name, const glm::mat3& value) const;
	void setMat4(const std::string& name, const glm::mat4& value) const;

	std::string vShaderName;
	std::string fShaderName;

private:
	GLuint pid = 0;
	bool verbose = true;
};

#endif
