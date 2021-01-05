#pragma once

#include "Graphics/Texture2D.h"
#include "Graphics/Mesh.hpp"
#include "Graphics/Shader.h" 
#include "Graphics/Font.h"

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

#include <switch.h>

class MapKorok;
class MapLocation;

class Map
{
public:
    Mesh<TextureVertex> m_Mesh;
    Texture2D m_Texture;
    Shader m_Shader;
    Font m_Font;

    glm::vec2 m_CameraPosition;
    float m_Zoom = 1.0f;

    glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
    glm::mat4 m_ViewMatrix = glm::mat4(1.0f);

    PadState* m_Pad;

    MapKorok* m_Koroks;
    MapLocation* m_Locations;

public:
    Map();

    void Update();
    void Render();

    ~Map();
};