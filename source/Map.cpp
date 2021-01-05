#include "Map.h"

#include <algorithm>
#include <switch.h>

#include "Graphics/BasicVertices.h"
#include "MapKorok.h"
#include "MapLocation.h"

#include "SavefileIO.h" 

Map::Map()
{
    m_Texture.Load("romfs:/BotW-Map-lowres.png");

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

    m_Mesh.m_Texture = &m_Texture;

    glm::vec3 vertexPositions[4];
    BasicVertices::Quad::Construct(vertexPositions, m_Texture.m_Width * 2, m_Texture.m_Height * 2);

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

    m_ProjectionMatrix = glm::ortho(-m_CameraWidth / 2, m_CameraWidth / 2, -m_CameraHeight / 2, m_CameraHeight / 2, -1.0f, 1.0f);

    // Load font
    m_Font.Load("romfs:/arial.ttf");
    m_Font.m_ProjectionMatrix = &m_ProjectionMatrix;
    m_Font.m_ViewMatrix = &m_ViewMatrix;

    // Create koroks
    m_Koroks = new MapKorok[900];
    for (int i = 0; i < 900; i++)
    {
        m_Koroks[i].m_Map = this;
        // The data has down being positive and up being negative. This renderer uses the opposite, so reverse the koroks y-coordinate
        m_Koroks[i].m_Position = glm::vec2(Data::Koroks[i].x, -Data::Koroks[i].y);

        m_Koroks[i].m_KorokData = &Data::Koroks[i];

        // Check if the korok has been found (if the found vector contains it)
        if (std::find(SavefileIO::foundKoroks.begin(), SavefileIO::foundKoroks.end(), &Data::Koroks[i]) != SavefileIO::foundKoroks.end()) {
            m_Koroks[i].m_Found = true;
        }
    }

    // Create locations
    m_Locations = new MapLocation[187];
    for (int i = 0; i < 187; i++)
    {
        m_Locations[i].m_Map = this;
        m_Locations[i].m_Font = &m_Font;
        
        // The data has down being positive and up being negative. This renderer uses the opposite, so reverse the koroks y-coordinate
        m_Locations[i].m_Position = glm::vec2(Data::Locations[i].x, -Data::Locations[i].y);

        m_Locations[i].m_LocationData = &Data::Locations[i];

        m_Locations[i].Init();

        // Check if the korok has been found (if the found vector contains it)
        if (std::find(SavefileIO::visitedLocations.begin(), SavefileIO::visitedLocations.end(), &Data::Locations[i]) != SavefileIO::visitedLocations.end()) {
            m_Locations[i].m_Found = true;
        }
    }
        
}

void Map::Update()
{
    u64 keysDown = padGetButtons(m_Pad);

    float zoomAmount = 0.01f;

    if (keysDown & HidNpadButton_R)
        m_Zoom *= 1.0f + zoomAmount;

    if (keysDown & HidNpadButton_L)
        m_Zoom *= 1.0f - zoomAmount;

    if (m_Zoom < 0.05f) m_Zoom = 0.05f;

    if (keysDown & HidNpadButton_A)
        m_CameraPosition.x += 10.0f / m_Zoom;

    if (keysDown & HidNpadButton_Y)
        m_CameraPosition.x -= 10.0f / m_Zoom;

    if (keysDown & HidNpadButton_X)
        m_CameraPosition.y += 10.0f / m_Zoom;

    if (keysDown & HidNpadButton_B)
        m_CameraPosition.y -= 10.0f / m_Zoom;

    m_ViewMatrix = glm::mat4(1.0f); // Reset (important)
    m_ViewMatrix = glm::scale(m_ViewMatrix, glm::vec3(m_Zoom, m_Zoom, 0.0f));
    m_ViewMatrix = glm::translate(m_ViewMatrix, glm::vec3(-m_CameraPosition, 1.0));

    // Update koroks
    for (int i = 0; i < 900; i++)
        m_Koroks[i].Update();

    // Update locations
    for (int i = 0; i < 187; i++)
        m_Locations[i].Update();
}

void Map::Render()
{
    m_Shader.Bind();

    m_Shader.SetUniform("u_ProjectionMatrix", m_ProjectionMatrix);
    m_Shader.SetUniform("u_ViewMatrix", m_ViewMatrix);
    m_Shader.SetUniform("u_Zoom", m_Zoom);

    m_Mesh.Render();

    m_Shader.Unbind();

    for (int i = 0; i < 900; i++)
        m_Koroks[i].Render();

    for (int i = 0; i < 187; i++)
        m_Locations[i].Render();
}

bool Map::IsInView(glm::vec2 position, float margin = 100.0f)
{
    // Calculate camera bounds
    float viewLeft = m_CameraPosition.x - (m_CameraWidth / 2) / m_Zoom - margin;
    float viewRight = m_CameraPosition.x + (m_CameraWidth / 2) / m_Zoom + margin;
    float viewBottom = m_CameraPosition.y - (m_CameraHeight / 2) / m_Zoom - margin;
    float viewTop = m_CameraPosition.y + (m_CameraHeight / 2) / m_Zoom + margin;

    // Check if text would be outside of view (horizontal)
    if (position.x < viewLeft || position.x > viewRight)
        return false;

    // Check if text would be outside of view (vertical)
    if (position.y < viewBottom || position.y > viewTop)
        return false;

    return true;
}

Map::~Map()
{
    delete[] m_Koroks;
    delete[] m_Locations;
}