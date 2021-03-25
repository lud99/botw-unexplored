#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <unordered_map>

#include <string>

class Shader
{
public:
	Shader();
	Shader(unsigned int shaderProgramId);

	int GetUniformLocation(const std::string& name);

	void Bind();
	void Unbind();

    void Delete();

	void SetUniform(const std::string& name, glm::vec2 vector);
	void SetUniform(const std::string& name, glm::ivec2 vector);
	void SetUniform(const std::string& name, glm::vec3 vector);
	void SetUniform(const std::string& name, glm::ivec3 vector);
    void SetUniform(const std::string& name, glm::vec4 vector);
	void SetUniform(const std::string& name, glm::mat4 matrix);
	void SetUniform(const std::string& name, float value);
	void SetUniform(const std::string& name, int value);

	~Shader();

public:
	unsigned int m_id = 0;

private:
	std::unordered_map<std::string, int> m_UniformLocations;
};

namespace ShaderLoader
{
	std::string ParseShader(const std::string& filepath);
	unsigned int CompileShader(unsigned int type, const std::string& source);

	Shader CreateShader(const std::string& vertexPath, const std::string& fragmentPath);
    Shader CreateShaderFromSource(const std::string& vertexSource, const std::string& fragmentSource);
    Shader CreateShader();
};