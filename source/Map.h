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
class Legend;
class Map
{
public:
    Mesh<TextureVertex> m_Mesh;
    Texture2D m_Texture;
    Shader m_Shader;
    Font m_Font;

    const float m_DefaultZoom = 0.5f;
    float m_Zoom = m_DefaultZoom;

    glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
    glm::mat4 m_ViewMatrix = glm::mat4(1.0f);

    glm::vec2 m_CameraPosition = glm::vec2(0.0f, 0.0f);
    glm::vec2 m_PrevCameraPosition;
    const float m_CameraWidth = 1280.0f;
    const float m_CameraHeight = 720.0f;

    int m_PrevTouchCount = 0;
    glm::vec2 m_PrevTouchPosition;
    glm::vec2 m_StartDragPos;
    bool m_IsDragging = false;
    bool m_IsLegendOpen = true;

    PadState* m_Pad;

    MapKorok* m_Koroks;
    MapLocation* m_Locations;

    Legend* m_Legend;

public:
    Map();

    void Update();
    void Render();

    bool IsInView(glm::vec2 position, float margin);

    ~Map();
};