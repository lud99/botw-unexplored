#pragma once

#include "Graphics/Texture2D.h"
#include "Graphics/Mesh.hpp"
#include "Graphics/Shader.h" 
#include "Graphics/Font.h"

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

#include <switch.h>

namespace Data
{
    struct Location;
};

class MapLocation
{
public:
    Data::Location* m_LocationData;

    glm::vec2 m_Position;
    glm::vec3 m_Color = glm::vec3(1.0f, 1.0f, 1.0f);

    bool m_Found = false;
    bool m_ShowAnyway = false;

    float m_Scale = 1.0f;

public:
    MapLocation();

    void Init();

    void Update();
    void Render();

    ~MapLocation();
};