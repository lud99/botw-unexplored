#include "MapKorok.h"

#include <switch.h>

#include "Map.h"
#include "Graphics/BasicVertices.h"
#include "SavefileIO.h"

MapKorok::MapKorok()
{
    // Only load one texture for all the 900 koroks
    if (!m_Texture)
        m_Texture = new Texture2D("romfs:/Korok-Seed.png");

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
            uniform mat4 u_ModelMatrix = mat4(1.0);
            uniform float u_Scale;

            void main()
            {
                gl_Position = u_ProjectionMatrix * u_ViewMatrix * u_ModelMatrix * vec4(position * u_Scale, 1.0);

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
    }

    m_Mesh.m_Texture = m_Texture;

    glm::vec3 vertexPositions[4];
    BasicVertices::Quad::Construct(vertexPositions, m_Texture->m_Width, m_Texture->m_Height);

    for (int i = 0; i < 4; i++)
    {
        TextureVertex vertex;
        vertex.position = vertexPositions[i];
        vertex.textureCoord = BasicVertices::Quad::TextureCoordinates[i];
        m_Mesh.AddVertex(vertex);
    }

    for (int i = 0; i < 6; i++)
        m_Mesh.AddIndex(BasicVertices::Quad::Indices[i]);

    m_Mesh.Update();
}

void MapKorok::Update()
{
    m_Scale = 1.0f / m_Map->m_Zoom;
}

void MapKorok::Render()
{
    if (m_Found) return;

    // Culling 
    float margin = 30.0f; // The width of the texture (korok size)

    // Don't render if not in view
    if (!m_Map->IsInView(m_Position, margin)) 
        return;

    glm::mat4 modelMatrix(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(m_Position, 1.0));

    m_Shader.Bind();

    m_Shader.SetUniform("u_ProjectionMatrix", m_Map->m_ProjectionMatrix);
    m_Shader.SetUniform("u_ViewMatrix", m_Map->m_ViewMatrix);
    m_Shader.SetUniform("u_ModelMatrix", modelMatrix);
    m_Shader.SetUniform("u_Scale", m_Scale);

    m_Mesh.Render();

    m_Shader.Unbind();
}

MapKorok::~MapKorok()
{
    if (m_Texture)
        delete m_Texture;
}

Texture2D* MapKorok::m_Texture;
Shader MapKorok::m_Shader;
Map* MapKorok::m_Map;