#pragma once

#include <string>
#include <map>
#include <glm/vec2.hpp>

#include "Shader.h"
#include "Mesh.hpp"

#define ALIGN_LEFT 0
#define ALIGN_CENTER 1
#define ALIGN_RIGHT 2

struct Text
{
    Text(unsigned char ch, glm::vec2 p, float s, glm::vec3 c, int a) :
    character(ch), position(p), scale(s), color(c), align(a) {};

    unsigned char character;
    glm::vec2 position = glm::vec2(0.0f, 0.0f);
    float scale = 1.0f;
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    int align = ALIGN_LEFT;
};
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
    Mesh<TextureVertex> m_CharMesh;
    
    std::map<char, Character> m_Characters;
    glm::mat4* m_ProjectionMatrix;
    glm::mat4* m_ViewMatrix;

    bool m_Initialized = false;

    std::map<char, std::vector<Text>> m_CharsToRender;

private:
    unsigned int m_Vao;
    unsigned int m_Vbo;

public:
    int Load(const std::string& filepath);

    void BeginBatch();

    glm::vec2 AddTextToBatch(const std::string& text, glm::vec2 position, float scale = 1.0f, glm::vec3 color = glm::vec3(1.0f), int align = ALIGN_LEFT);

    void RenderBatch();

    glm::vec2 RenderText(const std::string& text, glm::vec2 position, float scale, glm::vec3 color, int align = ALIGN_LEFT);
    
    ~Font();
};