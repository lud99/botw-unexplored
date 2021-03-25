#pragma once

#include <glad/glad.h>

#include <string>

class Texture2D
{
public:
	Texture2D();
	Texture2D(const std::string& filepath);

	void Load(const std::string& filepath);
	
	void Bind();
	void Unbind();

	~Texture2D();

public:
	unsigned int m_Texture = 0;

	int m_Width = 0;
	int m_Height = 0;
	int m_NrChannels = 0;
};

