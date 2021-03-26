#include "Texture2D.h"

#include <glad/glad.h>
#include <stb/stb_image.h>

#include "../Log.h"

Texture2D::Texture2D()
{

}

Texture2D::Texture2D(const std::string& filepath)
{
	Load(filepath);
}

void Texture2D::Load(const std::string& filepath)
{
	glGenTextures(1, &m_Texture);
	glBindTexture(GL_TEXTURE_2D, m_Texture);

	// set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	stbi_set_flip_vertically_on_load(true);

	// load and generate the texture
	unsigned char* data = stbi_load(filepath.c_str(), &m_Width, &m_Height, &m_NrChannels, 0);
	if (data)
	{
        //std::cout << "Loaded texture " << filepath << "...\n";

        if (m_NrChannels == 3)
		    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        if (m_NrChannels == 4)
		    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
	else
	{
        if (filepath != "")
		    Log("Failed to load texture", filepath);
        else 
            Log("Failed to load texture (empty path)");
	}

	stbi_image_free(data);
}

void Texture2D::Bind()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_Texture);
}

void Texture2D::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture2D::~Texture2D()
{
    glDeleteTextures(1, &m_Texture);
}
