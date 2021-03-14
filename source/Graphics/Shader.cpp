#include "Shader.h"

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <sstream>

Shader::Shader()
{
}

Shader::Shader(unsigned int shaderProgramId) : m_id(shaderProgramId)
{
}

int Shader::GetUniformLocation(const std::string& name)
{
	// Return it if it's already cached
	if (m_UniformLocations.count(name) > 0)
		return m_UniformLocations[name];

	int location = glGetUniformLocation(m_id, name.c_str());

	m_UniformLocations[name] = location;

	return location;
}

void Shader::Bind()
{
	glUseProgram(m_id);
}

void Shader::Unbind()
{
	glUseProgram(0);
}

void Shader::SetUniform(const std::string& name, glm::vec2 vector)
{
	glUniform2f(GetUniformLocation(name.c_str()), vector.x, vector.y);
}

void Shader::SetUniform(const std::string& name, glm::ivec2 vector)
{
	glUniform2i(GetUniformLocation(name.c_str()), vector.x, vector.y);
}

void Shader::SetUniform(const std::string& name, glm::vec3 vector)
{
	glUniform3f(GetUniformLocation(name.c_str()), vector.x, vector.y, vector.z);
}

void Shader::SetUniform(const std::string& name, glm::ivec3 vector)
{
	glUniform3i(GetUniformLocation(name.c_str()), vector.x, vector.y, vector.z);
}

void Shader::SetUniform(const std::string& name, glm::vec4 vector)
{
    glUniform4f(GetUniformLocation(name.c_str()), vector.x, vector.y, vector.z, vector.w);
}

void Shader::SetUniform(const std::string& name, glm::mat4 matrix)
{
	glUniformMatrix4fv(GetUniformLocation(name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::SetUniform(const std::string& name, float value)
{
	glUniform1f(GetUniformLocation(name.c_str()), value);
}

void Shader::SetUniform(const std::string& name, int value)
{
	glUniform1i(GetUniformLocation(name.c_str()), value);
}

void Shader::Delete()
{
    glDeleteProgram(m_id);

    m_id = 0;
}

Shader::~Shader()
{

}

std::string ShaderLoader::ParseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);

	//assert(!stream.is_open() && "Invalid file specified");

	std::string line;
	std::stringstream shaderStringStream;

	while (getline(stream, line)) {
		shaderStringStream << line << "\n";
	}

    printf(shaderStringStream.str().c_str());

	return shaderStringStream.str(); 
}

Shader ShaderLoader::CreateShader(const std::string& vertexPath, const std::string& fragmentPath)
{
	unsigned int program = glCreateProgram();
	unsigned int vertex = CompileShader(GL_VERTEX_SHADER, ParseShader(vertexPath));
	unsigned int fragment = CompileShader(GL_FRAGMENT_SHADER, ParseShader(fragmentPath));

	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vertex);
	glDeleteShader(fragment);

    //printf("Created shader program with the id %u\n", program);

	return Shader(program);
}

Shader ShaderLoader::CreateShaderFromSource(const std::string& vertexSource, const std::string& fragmentSource)
{
	unsigned int program = glCreateProgram();
	unsigned int vertex = CompileShader(GL_VERTEX_SHADER, vertexSource);
	unsigned int fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vertex);
	glDeleteShader(fragment);

    //printf("Created shader program with the id %u\n", program);

	return Shader(program);
}

Shader ShaderLoader::CreateShader()
{
	return Shader(0);
}

unsigned int ShaderLoader::CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();

	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	// Get the compilation status
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);

	// If the shader couldn't compile
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

		// Create char array
		char* message = new char[length];

		glGetShaderInfoLog(id, length, &length, message); // Get the actual message

		std::cout << "Failed to compile shader " << source << std::endl;
		std::cout << message << std::endl;

		glDeleteShader(id);

		return 0;
	}

	return id;
}