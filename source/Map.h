#pragma once

#include "Graphics/Texture2D.h"
#include "Graphics/Mesh.hpp"
#include "Graphics/Shader.h" 
#include "Graphics/Font.h"

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

#include <switch.h>

#include "Data.h"

template <typename T> class MapObject;

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
    extern bool m_IsInitialized;

    extern PadState* m_Pad;

    extern MapObject<Data::Korok>* m_Koroks;
    //extern MapObject<Data::Korok>* m_Shrines;
    extern MapObject<Data::Hinox>* m_Hinoxes;
    extern MapObject<Data::Talus>* m_Taluses;
    extern MapObject<Data::Molduga>* m_Moldugas;

    extern MapLocation* m_Locations;

    extern Legend* m_Legend;
};