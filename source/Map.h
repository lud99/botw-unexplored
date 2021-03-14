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
namespace Map
{
    void Init();

    void Update();
    void Render();

    bool IsInView(glm::vec2 position, float margin);

    void Destory();

    const float m_DefaultZoom = 0.5f;
    const float m_CameraWidth = 1280.0f;
    const float m_CameraHeight = 720.0f;

    extern Mesh<TextureVertex> m_Mesh;
    extern Texture2D m_Texture;
    extern Shader m_Shader;
    extern Font m_Font;

    extern float m_Zoom;

    extern glm::mat4 m_ProjectionMatrix;
    extern glm::mat4 m_ViewMatrix;

    extern glm::vec2 m_CameraPosition;
    extern glm::vec2 m_PrevCameraPosition;

    extern int m_PrevTouchCount;
    extern glm::vec2 m_PrevTouchPosition;
    extern glm::vec2 m_StartDragPos;
    extern bool m_IsDragging;
    extern bool m_IsLegendOpen;

    extern PadState* m_Pad;

    extern MapKorok* m_Koroks;
    extern MapLocation* m_Locations;

    extern Legend* m_Legend;
};