#pragma once

#include "Mesh.hpp"
#include "Shader.h" 

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

#include <switch.h>

class Texture2D;

class Quad
{
public:
    Quad();

    void Create(glm::vec2 bottomLeft, glm::vec2 bottomRight, glm::vec2 topRight, glm::vec2 topLeft);

    void Render();

    ~Quad();

public:
    Mesh<BasicVertex> m_Mesh;
    static Shader m_Shader;

    glm::mat4* m_ProjectionMatrix = nullptr;
    glm::mat4* m_ViewMatrix = nullptr;

    // bottomLeft, bottomRight, topRight, topLeft
    glm::vec2 m_Corners[4];

    glm::vec2 m_Position = glm::vec3(0.0f); 
    glm::vec4 m_Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
};

class TexturedQuad
{
public:
    TexturedQuad();

    void Create(const std::string& texturePath);
    void Create(const std::string& texturePath, glm::vec2 bottomLeft, glm::vec2 bottomRight, glm::vec2 topRight, glm::vec2 topLeft);

    void Render();

    ~TexturedQuad();

public:
    Mesh<TextureVertex> m_Mesh;
    Texture2D* m_Texture = nullptr;
    static Shader m_Shader;

    glm::mat4* m_ProjectionMatrix = nullptr;
    glm::mat4* m_ViewMatrix = nullptr;

    // bottomLeft, bottomRight, topRight, topLeft
    glm::vec2 m_Corners[4];

    glm::vec2 m_Position = glm::vec3(0.0f); 
    float m_Scale = 1.0f;
};