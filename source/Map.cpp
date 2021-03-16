#include "Map.h"

#include <algorithm>
#include <switch.h>

#include "Graphics/BasicVertices.h"
#include "MapLocation.h"
#include "Legend.h"
#include "Dialog.h"
#include "MapObject.hpp"

#include "SavefileIO.h" 

void Map::Init()
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

    // Create UI
    m_Legend = new Legend();
    m_Dialog = new Dialog(glm::vec2(0.0f, 0.0f), 700.0f, 400.0f, Dialog::InvalidSavefile);
    m_GameRunningDialog = new Dialog(glm::vec2(0.0f, 0.0f), 700.0f, 400.0f, Dialog::GameIsRunning);

    // Create koroks
    m_Koroks = new MapObject<Data::Korok>[Data::KoroksCount];
    MapObject<Data::Korok>::Init("romfs:/korokseed.png", Data::KoroksCount);

    // Create hinoxes
    m_Hinoxes = new MapObject<Data::Hinox>[Data::HinoxesCount];
    MapObject<Data::Hinox>::Init("romfs:/hinox.png", Data::HinoxesCount);
    
    // Create taluses
    m_Taluses = new MapObject<Data::Talus>[Data::TalusesCount];
    MapObject<Data::Talus>::Init("romfs:/talus.png", Data::TalusesCount);

    // Create moldugas
    m_Moldugas = new MapObject<Data::Molduga>[Data::MoldugasCount];
    MapObject<Data::Molduga>::Init("romfs:/molduga.png", Data::MoldugasCount);

    // Create locations
    m_Locations = new MapLocation[187];
    // for (int i = 0; i < 187; i++)
    // {
    //     // The data has down being positive and up being negative. This renderer uses the opposite, so reverse the koroks y-coordinate
    //     m_Locations[i].m_Position = glm::vec2(Data::Locations[i].x, -Data::Locations[i].y) * 0.5f;

    //     m_Locations[i].m_LocationData = &Data::Locations[i];

    //     m_Locations[i].Init();

    //     // Check if the korok has been found (if the found vector contains it)
    //     if (std::find(SavefileIO::visitedLocations.begin(), SavefileIO::visitedLocations.end(), &Data::Locations[i]) != SavefileIO::visitedLocations.end()) {
    //         m_Locations[i].m_Found = true;
    //     }
    // }

    UpdateMapObjects();

    m_IsInitialized = true;
}

void Map::UpdateMapObjects()
{
    if (!SavefileIO::LoadedSavefile)
        return;

    for (int i = 0; i < Data::KoroksCount; i++) // Korok
    {
        m_Koroks[i].m_Position = glm::vec2(Data::Koroks[i].x, -Data::Koroks[i].y) * 0.5f;

        // Check if the korok has been found (if the found vector contains it)
        if (std::find(SavefileIO::foundKoroks.begin(), SavefileIO::foundKoroks.end(), &Data::Koroks[i]) != SavefileIO::foundKoroks.end()) {
            m_Koroks[i].m_Found = true;
        }
    }

    for (int i = 0; i < Data::HinoxesCount; i++) // Hinox
    {
        m_Hinoxes[i].m_Position = glm::vec2(Data::Hinoxes[i].x, -Data::Hinoxes[i].y) * 0.5f;

        // Check if the korok has been found (if the found vector contains it)
        if (std::find(SavefileIO::defeatedHinoxes.begin(), SavefileIO::defeatedHinoxes.end(), &Data::Hinoxes[i]) != SavefileIO::defeatedHinoxes.end()) {
            m_Hinoxes[i].m_Found = true;
        }
    }

    for (int i = 0; i < Data::TalusesCount; i++) // Talus
    {
        m_Taluses[i].m_Position = glm::vec2(Data::Taluses[i].x, -Data::Taluses[i].y) * 0.5f;

        // Check if the korok has been found (if the found vector contains it)
        if (std::find(SavefileIO::defeatedTaluses.begin(), SavefileIO::defeatedTaluses.end(), &Data::Taluses[i]) != SavefileIO::defeatedTaluses.end()) {
            m_Taluses[i].m_Found = true;
        }
    }

    for (int i = 0; i < Data::MoldugasCount; i++) // Molduga
    {
        // The data has down being positive and up being negative. This renderer uses the opposite, so reverse the koroks y-coordinate
        m_Moldugas[i].m_Position = glm::vec2(Data::Moldugas[i].x, -Data::Moldugas[i].y) * 0.5f;

        // Check if the korok has been found (if the found vector contains it)
        if (std::find(SavefileIO::defeatedMoldugas.begin(), SavefileIO::defeatedMoldugas.end(), &Data::Moldugas[i]) != SavefileIO::defeatedMoldugas.end()) {
            m_Moldugas[i].m_Found = true;
        }
    }
}

void Map::Update()
{
    if (m_Pad == nullptr) return;

    u64 buttonsPressed = padGetButtonsDown(m_Pad);
    u64 buttonsDown = padGetButtons(m_Pad);
    u64 buttonsUp = padGetButtonsUp(m_Pad);

    float zoomAmount = 0.01f;
    float dragAmont = 0.85f;
    float analogStickMovementSpeed = 10.0f;
    float minZoom = 0.1f;

    // Zoom
    if (buttonsDown & HidNpadButton_R) // Zoom in
        m_Zoom *= 1.0f + zoomAmount;

    if (buttonsDown & HidNpadButton_L) // Zoom out
        m_Zoom *= 1.0f - zoomAmount;

    if (buttonsPressed & HidNpadButton_Minus)
        m_Zoom = m_DefaultZoom;

    if (m_Zoom < minZoom) m_Zoom = minZoom;

    // Show all koroks and locations
    if (buttonsPressed & HidNpadButton_X)
    {
        if (!m_Dialog->m_IsOpen)
            m_IsLegendOpen = !m_IsLegendOpen;
    }

    // Show only those that are found
    // if (buttonsUp & HidNpadButton_X)
    // {
    //     for (int i = 0; i < 900; i++)
    //         m_Koroks[i].m_ShowAnyway = false;

    //     for (int i = 0; i < 187; i++)
    //         m_Locations[i].m_ShowAnyway = false;
    // }

    // Analog stick camera movement
    // Read the sticks' position
    HidAnalogStickState analog_stick_l = padGetStickPos(m_Pad, 0);

    // Get the stick position between -1.0f and 1.0f, instead of -32767 and 32767
    glm::vec2 stickLPosition = glm::vec2((float)analog_stick_l.x / (float)JOYSTICK_MAX, (float)analog_stick_l.y / (float)JOYSTICK_MAX);
   
    float deadzone = 0.1f;
    float distanceToCenter = glm::distance(stickLPosition, glm::vec2(0.0f, 0.0f));
    if (distanceToCenter >= deadzone)
        m_CameraPosition += stickLPosition * (analogStickMovementSpeed / m_Zoom);

    // Dragging
    HidTouchScreenState state={0};
    if (hidGetTouchScreenStates(&state, 1)) {
        // Convert to more suitable coords
        glm::vec2 touchPosition = glm::vec2(state.touches[0].x - m_CameraWidth / 2, state.touches[0].y - m_CameraHeight / 2); 

        // A new touch
        if (state.count != m_PrevTouchCount)
        {   
            m_PrevTouchCount = state.count;

            // Dont drag if finger is on the legend
            if (!(m_IsLegendOpen && m_Legend->IsPositionOnLegend(touchPosition)) && 
                !(m_Dialog->m_IsOpen && m_Dialog->IsPositionOn(touchPosition)) &&
                !(m_GameRunningDialog->m_IsOpen && m_Dialog->IsPositionOn(touchPosition)))
            {
                // Check if the finger was pressed
                if (state.count == 1)
                {
                    m_IsDragging = true;
                    m_PrevTouchPosition = touchPosition; // The origin of the drag
                }
            }
                
            // Check if the finger was released
            if (state.count == 0)
                m_IsDragging = false;
        }

        // Handle the camera dragging
        if (state.count >= 1 && m_IsDragging)
        {
            // Calculate how much the finger has moved this frame
            glm::vec2 delta = m_PrevTouchPosition - touchPosition;

            // Move the camera by the delta. Flip the direction of the y-coordinate and 
            // divide by the zoom to move the same amount irregardless of the zoom
            m_CameraPosition += (delta * glm::vec2(1, -1) * dragAmont) / m_Zoom;

            // Set the touch pos to the most recent one, so we only check for the delta between each frame and not from when the drag started
            m_PrevTouchPosition = touchPosition;
        }
    }

    m_ViewMatrix = glm::mat4(1.0f); // Reset (important)
    m_ViewMatrix = glm::scale(m_ViewMatrix, glm::vec3(m_Zoom, m_Zoom, 0.0f));
    m_ViewMatrix = glm::translate(m_ViewMatrix, glm::vec3(-m_CameraPosition, 1.0));

    if (m_IsLegendOpen) 
        m_Legend->Update();

    if (m_Dialog->m_IsOpen) 
        m_Dialog->Update();
    if (m_GameRunningDialog->m_IsOpen)
        m_GameRunningDialog->Update();

    // Update objects
    if (SavefileIO::LoadedSavefile)
    {
        for (int i = 0; i < Data::KoroksCount; i++)
            m_Koroks[i].Update();
        for (int i = 0; i < Data::HinoxesCount; i++)
            m_Hinoxes[i].Update();
        for (int i = 0; i < Data::TalusesCount; i++)
            m_Taluses[i].Update();
        for (int i = 0; i < Data::MoldugasCount; i++)
            m_Moldugas[i].Update();

        // Update locations
        // for (int i = 0; i < 187; i++)
        //      m_Locations[i].Update();
    }

    m_PrevCameraPosition = m_CameraPosition;
}

void Map::Render()
{
    m_Shader.Bind();

    m_Shader.SetUniform("u_ProjectionMatrix", m_ProjectionMatrix);
    m_Shader.SetUniform("u_ViewMatrix", m_ViewMatrix);
    m_Shader.SetUniform("u_Zoom", m_Zoom);

    m_Mesh.Render();

    m_Shader.Unbind();

    if (SavefileIO::LoadedSavefile)
    {
        if (m_Legend->m_Show[IconButton::ButtonTypes::Koroks])
            MapObject<Data::Korok>::Render();
        if (m_Legend->m_Show[IconButton::ButtonTypes::Hinoxes])
            MapObject<Data::Hinox>::Render();
        if (m_Legend->m_Show[IconButton::ButtonTypes::Taluses])
            MapObject<Data::Talus>::Render();
        if (m_Legend->m_Show[IconButton::ButtonTypes::Moldugas])
           MapObject<Data::Molduga>::Render();
    }

    if (m_IsLegendOpen) 
        m_Legend->Render();

    if (m_Dialog->m_IsOpen) 
        m_Dialog->Render();
    if (m_GameRunningDialog->m_IsOpen)
        m_GameRunningDialog->Render();

    glm::mat4 emptyViewMatrix(1.0);
    m_Font.m_ViewMatrix = &emptyViewMatrix; // Don't draw the text relative to the camera 

    if (SavefileIO::GameIsRunning && SavefileIO::LoadedSavefile)
    {
        m_Font.RenderText("BotW is running.", glm::vec2(m_ScreenRight - 20, m_ScreenTop - 30), 0.5f, glm::vec3(1.0f), ALIGN_RIGHT);
        m_Font.RenderText("Loaded older save", glm::vec2(m_ScreenRight - 20, m_ScreenTop - 60), 0.5f, glm::vec3(1.0f), ALIGN_RIGHT);
    }
        

    // for (int i = 0; i < 187; i++)
    //     m_Locations[i].Render();

    //float textScale = 0.5f;

    //m_Font.RenderText("Press Y to open Legend", glm::vec2(0, 0), textScale, glm::vec3(1.0f) /* white */);

    // Render stats text
    // glm::mat4 emptyViewMatrix(1.0);
    // m_Font.m_ViewMatrix = &emptyViewMatrix; // Don't draw the text relative to the camera 
    
    // // Koroks
    // std::string koroksText = std::to_string(SavefileIO::foundKoroks.size()) + " / 900 koroks found";

    // glm::vec2 koroksPadding(25.0f, 25.0f);
    // glm::vec2 koroksPosition = glm::vec2(-m_CameraWidth / 2, -m_CameraHeight / 2) + koroksPadding;

    // // Render the text and get the width, so the next text can be relative to that
    // glm::vec2 koroksTextSize = m_Font.RenderText(koroksText, koroksPosition, textScale, glm::vec3(1.0f) /* white */);

    // // Locations
    // std::string locationsText = std::to_string(SavefileIO::visitedLocations.size()) + " / 187 locations visited";

    // glm::vec2 locationsPadding(25.0f, 0.0f);
    // glm::vec2 locationsPosition = koroksPosition + glm::vec2(koroksTextSize.x, 0.0f) /* only use the text width, not height */ + locationsPadding;

    // // Render the text
    // m_Font.RenderText(locationsText, locationsPosition, textScale, glm::vec3(1.0f) /* white */);

    // glm::vec2 showAllPosition(-m_CameraWidth / 2 + koroksPadding.x, m_CameraHeight / 2 - (koroksTextSize.y + koroksPadding.y));

    // // Render the text
    // m_Font.RenderText("Hold X to show all koroks and locations, L and R to zoom, - to reset zoom, + to exit", showAllPosition, textScale, glm::vec3(1.0f) /* white */);

    m_Font.m_ViewMatrix = &m_ViewMatrix;
}

bool Map::IsInView(glm::vec2 position, float margin = 100.0f)
{
    // Calculate camera bounds
    float viewLeft = m_CameraPosition.x - (m_CameraWidth / 2) / m_Zoom - margin;
    float viewRight = m_CameraPosition.x + (m_CameraWidth / 2) / m_Zoom + margin;
    float viewBottom = m_CameraPosition.y - (m_CameraHeight / 2) / m_Zoom - margin;
    float viewTop = m_CameraPosition.y + (m_CameraHeight / 2) / m_Zoom + margin;

    // Check if the position would be outside of view (horizontal)
    if (position.x < viewLeft || position.x > viewRight)
        return false;

    // Check if the position would be outside of view (vertical)
    if (position.y < viewBottom || position.y > viewTop)
        return false;

    return true;
}

void Map::Destory()
{
    if (!m_IsInitialized) return;
    
    delete[] m_Koroks;
    delete[] m_Hinoxes;
    delete[] m_Taluses;
    delete[] m_Moldugas;
    delete[] m_Locations;

    delete m_Dialog;
    delete m_GameRunningDialog;
}

Mesh<TextureVertex> Map::m_Mesh;
Texture2D Map::m_Texture;
Shader Map::m_Shader;
Font Map::m_Font;

float Map::m_Zoom = Map::m_DefaultZoom;

glm::mat4 Map::m_ProjectionMatrix = glm::mat4(1.0f);
glm::mat4 Map::m_ViewMatrix = glm::mat4(1.0f);

glm::vec2 Map::m_CameraPosition = glm::vec2(0.0f, 0.0f);
glm::vec2 Map::m_PrevCameraPosition;

int Map::m_PrevTouchCount = 0;
glm::vec2 Map::m_PrevTouchPosition;
glm::vec2 Map::m_StartDragPos;
bool Map::m_IsDragging = false;
bool Map::m_IsLegendOpen = true;
bool Map::m_IsInitialized = false;
bool Map::m_ShouldExit = false;
bool Map::m_ShouldChooseProfile = false;

PadState* Map::m_Pad;
MapObject<Data::Korok>* Map::m_Koroks;
MapObject<Data::Hinox>* Map::m_Hinoxes;
MapObject<Data::Talus>* Map::m_Taluses;
MapObject<Data::Molduga>* Map::m_Moldugas;
MapLocation* Map::m_Locations;

Legend* Map::m_Legend;
Dialog* Map::m_Dialog;
Dialog* Map::m_GameRunningDialog;