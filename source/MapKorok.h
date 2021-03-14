#pragma once

#include "Graphics/Texture2D.h"
#include "Graphics/Mesh.hpp"
#include "Graphics/Shader.h" 

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

#include <switch.h>

namespace Data
{
    struct Korok;
};

class MapKorok
{
public:
    static Mesh<TextureVertex> m_Mesh;
    static Texture2D* m_Texture;
    static Shader m_Shader;

    Data::Korok* m_KorokData;

    glm::vec2 m_Position = glm::vec2(0.0f, 0.0f);
    bool m_Found = false;
    bool m_ShowAnyway = false;

    float m_Scale = 1.0f;

public:
    MapKorok();

    void AddToMesh();

    void Update();
    void Render();

    ~MapKorok();
};