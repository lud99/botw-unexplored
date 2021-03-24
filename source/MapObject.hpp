#pragma once

#include "Graphics/Mesh.hpp"
#include "Graphics/Shader.h" 
#include "Graphics/Texture2D.h"
#include "Graphics/BasicVertices.h"

#include "Map.h"

#include <glm/vec2.hpp>
#include <iostream>

class Texture2D;

template <typename T>
class MapObject
{
public:
    static Texture2D* m_Texture;
    static Shader m_Shader;
    static Mesh<TextureVertex> m_Mesh;

    T* m_ObjectData;

    glm::vec2 m_Position = glm::vec2(0.0f, 0.0f);
    bool m_Found = false;
    
    static bool m_ShowAnyway;

    static float m_Scale;

public:
    MapObject();

    static void Init(const std::string& path, int count);

    void AddToMesh();

    void Update(bool clear = false);
    static void Render();

    bool IsClicked(glm::vec2 position);

    ~MapObject();
};

template <typename T>
MapObject<T>::MapObject()
{
    
}

template <typename T>
void MapObject<T>::Init(const std::string& path, int count)
{
    // Only load one texture
    if (!m_Texture)
        m_Texture = new Texture2D(path);

    // Only create shader if it's the first 
    if (m_Shader.m_id == 0)
    {
        std::string vertexShaderSource = R"text(
            #version 330 core

            layout (location = 0) in vec3 position;
            layout (location = 1) in vec4 color;
            layout (location = 2) in vec2 texCoord;

            out vec2 passTextureCoord;

            uniform mat4 u_ProjectionMatrix = mat4(1.0);
            uniform mat4 u_ViewMatrix = mat4(1.0);

            void main()
            {
                gl_Position = u_ProjectionMatrix * u_ViewMatrix * vec4(position, 1.0);

                passTextureCoord = texCoord;
            }
        )text";

        std::string fragmentShaderSource = R"text(
            #version 330 core

            layout (location = 0) out vec4 color;
            in vec2 passTextureCoord;

            uniform sampler2D tex;

            void main()
            {
                color = texture(tex, passTextureCoord);

                if (color.a == 0.0)
                    discard;
            }
        )text";

        m_Shader = ShaderLoader::CreateShaderFromSource(vertexShaderSource, fragmentShaderSource);

        m_Mesh.m_Texture = m_Texture;
        m_Mesh.m_UseDynamicBuffer = true;
        m_Mesh.m_DynamicBufferSize = sizeof(TextureVertex) * 4 * count;

        m_Mesh.CreateEmptyBuffer();
    }
}

template <typename T>
void MapObject<T>::AddToMesh()
{   
    glm::vec3 vertexPositions[4];
    BasicVertices::Quad::Construct(vertexPositions, m_Position, m_Texture->m_Width * m_Scale, m_Texture->m_Height * m_Scale);

    for (int i = 0; i < 4; i++)
    {
        TextureVertex vertex;
        vertex.position = vertexPositions[i];

        vertex.textureCoord = BasicVertices::Quad::TextureCoordinates[i];
        m_Mesh.AddVertex(vertex);
    }

    for (int i = 0; i < 6; i++)
        m_Mesh.AddIndex(BasicVertices::Quad::Indices[i] + m_Mesh.GetVertices().size() - 4);
}

template <typename T>
void MapObject<T>::Update(bool clear)
{
    m_Scale = 1.0f / Map::m_Zoom;

    if (clear) m_Mesh.Clear();

    if (m_Found && !Map::m_ShowAllObjects) 
        return;
    
    // Culling 
    float margin = m_Texture->m_Width + 10.0f;
    if (!Map::IsInView(m_Position, margin)) 
      return;

    // Set dynamic mesh
    AddToMesh();
}

template <typename T>
void MapObject<T>::Render()
{
    if (m_Mesh.GetVertices().empty())
        return;

    m_Mesh.Update();

    m_Shader.Bind();

    m_Shader.SetUniform("u_ProjectionMatrix", Map::m_ProjectionMatrix);
    m_Shader.SetUniform("u_ViewMatrix", Map::m_ViewMatrix);

    m_Mesh.Render();

    m_Shader.Unbind();

    // Clear the mesh
    m_Mesh.GetVertices().clear();
    m_Mesh.GetIndices().clear();
}

template <typename T>
bool MapObject<T>::IsClicked(glm::vec2 position)
{
    glm::vec2 worldPos = position / Map::m_Zoom + Map::m_CameraPosition;

    float width = m_Texture->m_Width * m_Scale;
    float height = m_Texture->m_Height * m_Scale; 

    if (worldPos.x > m_Position.x - width / 2 && worldPos.x < m_Position.x + width / 2)
    {
        if (worldPos.y > m_Position.y - height / 2 && worldPos.y < m_Position.y + height / 2)
        {
            return true;
        }
    }

    return false;
}

template <typename T>
MapObject<T>::~MapObject()
{
    // Delete the texture (only once)
    if (m_Texture) {
        delete m_Texture;

        m_Texture = nullptr;
    }

    // If the shader still exists
    if (m_Shader.m_id != 0)
        m_Shader.Delete();
}

template <typename T>
Texture2D* MapObject<T>::m_Texture;
template <typename T>
Shader MapObject<T>::m_Shader;
template <typename T>
Mesh<TextureVertex> MapObject<T>::m_Mesh;

template <typename T>
bool MapObject<T>::m_ShowAnyway = false;
template <typename T>
float MapObject<T>::m_Scale;