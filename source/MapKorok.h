#pragma once

#include "Graphics/Texture2D.h"
#include "Graphics/Mesh.hpp"
#include "Graphics/Shader.h" 

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

#include <switch.h>

class Map;

namespace Data
{
    struct Korok;
};

class MapKorok
{
public:
    Mesh<TextureVertex> m_Mesh;
    static Texture2D* m_Texture;
    static Shader m_Shader;

    static Map* m_Map;

    Data::Korok* m_KorokData;

    glm::vec2 m_Position;
    bool m_Found = false;

    float m_Scale = 1.0f;

public:
    MapKorok();

    void Update();
    void Render();

    ~MapKorok();
};