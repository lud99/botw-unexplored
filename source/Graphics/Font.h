#pragma once

#include <string>
#include <map>
#include <glm/vec2.hpp>

#include "Shader.h"
#include "Mesh.hpp"

class Text;

struct Character {
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Offset to advance to next glyph
};

class Font
{
public:
    Shader m_Shader;
    Mesh<TextureVertex> m_Mesh;
    
    std::map<char, Character> m_Characters;
    glm::mat4* m_ProjectionMatrix;
    glm::mat4* m_ViewMatrix;

    bool m_Initialized = false;

private:
    unsigned int m_Vao;
    unsigned int m_Vbo;

public:
    int Load(const std::string& filepath);

    glm::vec2 RenderText(const std::string& text, glm::vec2 position, float scale, glm::vec3 color);
    void RenderText(Text& text, glm::vec2 position, float scale, glm::vec3 color);
};

class Text
{
public:
    Mesh<TextureVertex> m_Mesh;
    Font* m_Font;

public:
    void Create(const std::string& text);
};