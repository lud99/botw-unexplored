#include "Map.h"

#include <algorithm>
#include <switch.h>

#include "Graphics/BasicVertices.h"
#include "Graphics/LineRenderer.h"
#include "Graphics/Quad.h"
#include "MapLocation.h"
#include "Legend.h"
#include "Dialog.h"
#include "MapObject.hpp"
#include "KorokDialog.h"
#include "Log.h"

#include "SavefileIO.h" 

constexpr float MapScale = 0.25f;

void Map::Init()
{
    Data::LoadPaths();

    m_ProjectionMatrix = glm::ortho(-m_CameraWidth / 2, m_CameraWidth / 2, -m_CameraHeight / 2, m_CameraHeight / 2, -1.0f, 1.0f);

    // Map image
    m_MapBackground.Create("romfs:/map-lowres.png");
    m_MapBackground.m_ProjectionMatrix = &m_ProjectionMatrix;
    m_MapBackground.m_ViewMatrix = &m_ViewMatrix; 

    // Load font
    m_Font.Load("romfs:/arial.ttf");
    m_Font.m_ProjectionMatrix = &m_ProjectionMatrix;
    m_Font.m_ViewMatrix = &m_ViewMatrix;

    m_LineRenderer = new LineRenderer();

    m_KorokDialog = new KorokDialog();

    // Create UI
    m_Legend = new Legend();
    m_NoSavefileDialog = new Dialog(glm::vec2(0.0f, 0.0f), 700.0f, 400.0f, Dialog::InvalidSavefile);
    m_GameRunningDialog = new Dialog(glm::vec2(0.0f, 0.0f), 700.0f, 400.0f, Dialog::GameIsRunning);
    m_MasterModeDialog = new Dialog(glm::vec2(0.0f, 0.0f), 700.0f, 400.0f, Dialog::MasterModeChoose);

    m_MasterModeIcon.Create("romfs:/mastermodeicon.png");
    m_MasterModeIcon.m_Position = glm::vec2(m_ScreenLeft + 45.0f, m_ScreenBottom + 40.0f);
    m_MasterModeIcon.m_Scale = 0.1f;
    m_MasterModeIcon.m_ProjectionMatrix = &m_ProjectionMatrix;
    m_MasterModeIcon.m_ViewMatrix = nullptr;

    // Create koroks
    m_Koroks = new MapObject<Data::Korok>[Data::KoroksCount];
    MapObject<Data::Korok>::Init("romfs:/korokseed.png", Data::KoroksCount);

    // Create shrines
    m_Shrines = new MapObject<Data::Shrine>[Data::ShrineCount];
    MapObject<Data::Shrine>::Init("romfs:/shrine.png", Data::ShrineCount);
    m_DLCShrines = new MapObject<Data::DLCShrine>[Data::DLCShrineCount];
    MapObject<Data::DLCShrine>::Init("romfs:/dlcshrine.png", Data::DLCShrineCount);

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
    m_Locations = new MapLocation[Data::LocationsCount];

    UpdateMapObjects();
}

void Map::UpdateMapObjects()
{
    if (!SavefileIO::LoadedSavefile)
        return;

    for (int i = 0; i < Data::KoroksCount; i++) // Korok
    {
        m_Koroks[i].m_Position = glm::vec2(Data::Koroks[i].x, -Data::Koroks[i].y) * MapScale;

        m_Koroks[i].m_ObjectData = &Data::Koroks[i];

        // Check if the korok has been found (if the found vector contains it)
        m_Koroks[i].m_Found = std::find(
            SavefileIO::foundKoroks.begin(), 
            SavefileIO::foundKoroks.end(), 
            &Data::Koroks[i]) != SavefileIO::foundKoroks.end();
    }

    for (int i = 0; i < Data::ShrineCount; i++) // Shrine
    {
        m_Shrines[i].m_Position = glm::vec2(Data::Shrines[i].x, -Data::Shrines[i].y) * MapScale;

        // Check if the korok has been found (if the found vector contains it)
        m_Shrines[i].m_Found = std::find(
            SavefileIO::foundShrines.begin(), 
            SavefileIO::foundShrines.end(), 
            &Data::Shrines[i]) != SavefileIO::foundShrines.end();
    }

    for (int i = 0; i < Data::DLCShrineCount; i++) // DLC Shrine
    {
        m_DLCShrines[i].m_Position = glm::vec2(Data::DLCShrines[i].x, -Data::DLCShrines[i].y) * MapScale;

        // Check if the korok has been found (if the found vector contains it)
        m_DLCShrines[i].m_Found = std::find(
            SavefileIO::foundDLCShrines.begin(), 
            SavefileIO::foundDLCShrines.end(), 
            &Data::DLCShrines[i]) != SavefileIO::foundDLCShrines.end();
    }

    for (int i = 0; i < Data::HinoxesCount; i++) // Hinox
    {
        m_Hinoxes[i].m_Position = glm::vec2(Data::Hinoxes[i].x, -Data::Hinoxes[i].y) * MapScale;

        // Check if the korok has been found (if the found vector contains it)
        m_Hinoxes[i].m_Found = std::find(
            SavefileIO::defeatedHinoxes.begin(), 
            SavefileIO::defeatedHinoxes.end(), 
            &Data::Hinoxes[i]) != SavefileIO::defeatedHinoxes.end();
    }

    for (int i = 0; i < Data::TalusesCount; i++) // Talus
    {
        m_Taluses[i].m_Position = glm::vec2(Data::Taluses[i].x, -Data::Taluses[i].y) * MapScale;

        // Check if the korok has been found (if the found vector contains it)
        m_Taluses[i].m_Found = std::find(
            SavefileIO::defeatedTaluses.begin(), 
            SavefileIO::defeatedTaluses.end(), 
            &Data::Taluses[i]) != SavefileIO::defeatedTaluses.end();
    }

    for (int i = 0; i < Data::MoldugasCount; i++) // Molduga
    {
        m_Moldugas[i].m_Position = glm::vec2(Data::Moldugas[i].x, -Data::Moldugas[i].y) * MapScale;

        // Check if the korok has been found (if the found vector contains it)
        m_Moldugas[i].m_Found = std::find(
            SavefileIO::defeatedMoldugas.begin(), 
            SavefileIO::defeatedMoldugas.end(), 
            &Data::Moldugas[i]) != SavefileIO::defeatedMoldugas.end();
    }

    for (int i = 0; i < Data::LocationsCount; i++) // Locations
    {
        m_Locations[i].m_Position = glm::vec2(Data::Locations[i].x, -Data::Locations[i].y) * MapScale;
        m_Locations[i].m_LocationData = &Data::Locations[i];

        // Check if the korok has been found (if the found vector contains it)
        m_Locations[i].m_Found = std::find(
            SavefileIO::visitedLocations.begin(), 
            SavefileIO::visitedLocations.end(), 
            &Data::Locations[i]) != SavefileIO::visitedLocations.end();
    }

    Log("Updated map objects");
}

void Map::Update()
{
    if (m_Pad == nullptr) return;

    u64 buttonsPressed = padGetButtonsDown(m_Pad);
    u64 buttonsDown = padGetButtons(m_Pad);

    float zoomAmount = 0.015f;
    float dragAmont = 0.85f;
    float analogStickMovementSpeed = 10.0f;
    float minZoom = 0.1f;
        
    // Handle zooming like BotW
    HidAnalogStickState analog_stick_r = padGetStickPos(m_Pad, 1);

    // Get the stick position between -1.0f and 1.0f, instead of -32767 and 32767
    glm::vec2 stickRPosition = glm::vec2((float)analog_stick_r.x / (float)JOYSTICK_MAX, (float)analog_stick_r.y / (float)JOYSTICK_MAX);
   
    float deadzone = 0.1f;
    if (fabs(stickRPosition.y) >= deadzone)
        m_Zoom *= 1.0f + zoomAmount * stickRPosition.y;

    // Zoom with L and R
    if (buttonsDown & HidNpadButton_R) // Zoom in
        m_Zoom *= 1.0f + zoomAmount;

    if (buttonsDown & HidNpadButton_L) // Zoom out
        m_Zoom *= 1.0f - zoomAmount;

    // Reset zoom if pressing L or R stick
    if (buttonsPressed & HidNpadButton_StickL)
    {
        m_Zoom = m_DefaultZoom;
        m_CameraPosition = glm::vec2(0.0f, 0.0f);
    }

    if (m_Zoom < minZoom) m_Zoom = minZoom;

    // Open profile picker
    if (buttonsPressed & HidNpadButton_Minus)
    {
        if (SavefileIO::LoadedSavefile)
        {
            m_LoadMasterMode = false;

            SavefileIO::LoadGamesave(false, true);
            UpdateMapObjects();
        }
    }

    // Toggle legend
    if (buttonsPressed & HidNpadButton_X)
    {
        // Close the korok dialog first, then if it's not open close the legend
        if (m_KorokDialog->m_IsOpen)
            m_KorokDialog->SetOpen(false);
        else if (!m_NoSavefileDialog->m_IsOpen)
            m_Legend->m_IsOpen = !m_Legend->m_IsOpen;
    }

    // Toggle showing everything
    // if (buttonsDown & HidNpadButton_B)
    //     m_ShowAllObjects = true;
    // if (buttonsUp & HidNpadButton_B)
    //     m_ShowAllObjects = false;

    // Toggle master mode
    if (buttonsPressed & HidNpadButton_Y)
    {
        if (SavefileIO::MostRecentMasterModeFile != -1)
        {
            m_LoadMasterMode = !m_LoadMasterMode;

            SavefileIO::LoadGamesave(m_LoadMasterMode);
            UpdateMapObjects();
        }
    }

    if (buttonsPressed & HidNpadButton_B)
    {
        if (m_KorokDialog->m_IsOpen) 
        {
            m_Koroks[m_KorokDialog->m_KorokIndex].m_Found = true;
            m_KorokDialog->SetOpen(false);
        }
    }

    // Analog stick camera movement
    // Read the sticks' position
    HidAnalogStickState analog_stick_l = padGetStickPos(m_Pad, 0);

    // Get the stick position between -1.0f and 1.0f, instead of -32767 and 32767
    glm::vec2 stickLPosition = glm::vec2((float)analog_stick_l.x / (float)JOYSTICK_MAX, (float)analog_stick_l.y / (float)JOYSTICK_MAX);
   
    float distanceToCenter = glm::distance(stickLPosition, glm::vec2(0.0f, 0.0f));
    if (distanceToCenter >= deadzone)
        m_CameraPosition += stickLPosition * (analogStickMovementSpeed / m_Zoom);

    m_ViewMatrix = glm::mat4(1.0f); // Reset (important)
    m_ViewMatrix = glm::scale(m_ViewMatrix, glm::vec3(m_Zoom, m_Zoom, 0.0f));
    m_ViewMatrix = glm::translate(m_ViewMatrix, glm::vec3(-m_CameraPosition, 1.0));

    // Dragging
    HidTouchScreenState state={0};
    if (hidGetTouchScreenStates(&state, 1)) {
        // Convert to more suitable coords
        glm::vec2 touchPosition = glm::vec2(state.touches[0].x - m_CameraWidth / 2, -(state.touches[0].y - m_CameraHeight / 2)); 

        // A new touch
        if (state.count != m_PrevTouchCount)
        {   
            m_PrevTouchCount = state.count;

            // Dont drag if finger is on the legend
            if (!(m_Legend->m_IsOpen && m_Legend->IsPositionOnLegend(touchPosition)) && 
                !(m_NoSavefileDialog->m_IsOpen && m_NoSavefileDialog->IsPositionOn(touchPosition)) &&
                !(m_GameRunningDialog->m_IsOpen && m_GameRunningDialog->IsPositionOn(touchPosition)) &&
                !(m_MasterModeDialog->m_IsOpen && m_MasterModeDialog->IsPositionOn(touchPosition)))
            {
                // Check if the finger was pressed
                if (state.count == 1)
                {   
                    // Check if clicked korok
                    bool clicked = false;
                    for (int i = 0; i < Data::KoroksCount; i++)
                    {
                        if ((!m_Koroks[i].m_Found || m_Legend->m_Show[IconButton::ShowCompleted]) && m_Koroks[i].IsClicked(touchPosition))
                        {
                            // Set the korok dialog
                            m_KorokDialog->SetSeed(m_Koroks[i].m_ObjectData->zeldaDungeonId, i);
                            m_KorokDialog->SetOpen(true);

                            m_Legend->m_IsOpen = false;

                            clicked = true;
                        }
                    }

                    // Hide the korok info if no korok was clicked on
                    if (!clicked)
                    {
                        //m_KorokDialog->SetOpen(false);
                    }
                    
                    // Only drag if not clicking on korok
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
            m_CameraPosition += (delta * dragAmont) / m_Zoom;

            // Set the touch pos to the most recent one, so we only check for the delta between each frame and not from when the drag started
            m_PrevTouchPosition = touchPosition;
        }
    }

    m_ViewMatrix = glm::mat4(1.0f); // Reset (important)
    m_ViewMatrix = glm::scale(m_ViewMatrix, glm::vec3(m_Zoom, m_Zoom, 0.0f));
    m_ViewMatrix = glm::translate(m_ViewMatrix, glm::vec3(-m_CameraPosition, 1.0));

    if (m_Legend->m_IsOpen) 
        m_Legend->Update();

    if (m_NoSavefileDialog->m_IsOpen) 
        m_NoSavefileDialog->Update();
    if (m_GameRunningDialog->m_IsOpen)
        m_GameRunningDialog->Update();
    if (m_MasterModeDialog->m_IsOpen)
        m_MasterModeDialog->Update();

    // Update objects
    if (SavefileIO::LoadedSavefile)
    {
        // Clear the meshes during the first iteration of the loop (i == 0) == true
        for (int i = 0; i < Data::KoroksCount; i++)
            m_Koroks[i].Update(i == 0);
        for (int i = 0; i < Data::ShrineCount; i++)
            m_Shrines[i].Update(i == 0);
        for (int i = 0; i < Data::DLCShrineCount; i++)
            m_DLCShrines[i].Update(i == 0);
        for (int i = 0; i < Data::HinoxesCount; i++)
            m_Hinoxes[i].Update(i == 0);
        for (int i = 0; i < Data::TalusesCount; i++)
            m_Taluses[i].Update(i == 0);
        for (int i = 0; i < Data::MoldugasCount; i++)
            m_Moldugas[i].Update(i == 0);
        for (int i = 0; i < Data::LocationsCount; i++)
            m_Locations[i].Update();

        MapLocation::m_ShowAnyway = m_Legend->m_Show[IconButton::ShowCompleted];
    }

    m_PrevCameraPosition = m_CameraPosition;
}

void Map::Render()
{
    m_MapBackground.Render();

    Map::m_Font.BeginBatch();
    
    if (SavefileIO::LoadedSavefile)
    {
        if (m_Legend->m_Show[IconButton::ButtonTypes::Koroks]) 
        {   
            // Render korok paths
            for (int k = 0; k < Data::KoroksCount; k++)
            {
                // This korok has no paths
                if (m_Koroks[k].m_ObjectData->path == nullptr)
                    continue;

                // Don't render if found
                if (m_Koroks[k].m_Found && !m_Legend->m_Show[IconButton::ShowCompleted])
                    continue;

                Data::KorokPath* path = m_Koroks[k].m_ObjectData->path;

                // 0 -> 1
                // 1 -> 2
                // 2 -> 3
                for (unsigned int p = 1; p < path->points.size(); p++)
                {
                    glm::vec2 start = path->points[p - 1] * MapScale;
                    start.y *= -1; // Flip the y coord
                    glm::vec2 end = path->points[p] * MapScale;
                    end.y *= -1;

                    float width = (1.0f / m_Zoom) * 2.0f;
                    if (m_Zoom >= 3.0f)
                        width = 0.75f;

                    m_LineRenderer->AddLine(start, end, width);
                }
            }

            m_LineRenderer->RenderLines(m_ProjectionMatrix, m_ViewMatrix);

            MapObject<Data::Korok>::Render();
        }
        if (m_Legend->m_Show[IconButton::ButtonTypes::Shrines])
            MapObject<Data::Shrine>::Render();
        if (m_Legend->m_Show[IconButton::ButtonTypes::Shrines] && SavefileIO::HasDLC)
            MapObject<Data::DLCShrine>::Render();
        if (m_Legend->m_Show[IconButton::ButtonTypes::Hinoxes])
            MapObject<Data::Hinox>::Render();
        if (m_Legend->m_Show[IconButton::ButtonTypes::Taluses])
            MapObject<Data::Talus>::Render();
        if (m_Legend->m_Show[IconButton::ButtonTypes::Moldugas])
           MapObject<Data::Molduga>::Render();
        if (m_Legend->m_Show[IconButton::ButtonTypes::Locations])
        {
            for (int i = 0; i < Data::LocationsCount; i++)
                m_Locations[i].Render();
        }
    }

    m_Font.RenderBatch();

    // Draw behind legend
    if (m_LoadMasterMode)
        m_MasterModeIcon.Render();

    m_Font.BeginBatch();
    if (m_Legend->m_IsOpen) 
        m_Legend->Render();

    if (m_NoSavefileDialog->m_IsOpen) 
        m_NoSavefileDialog->Render();
    if (m_GameRunningDialog->m_IsOpen)
        m_GameRunningDialog->Render();
    if (m_MasterModeDialog->m_IsOpen)
        m_MasterModeDialog->Render();

    if (!m_Legend->m_IsOpen && !m_KorokDialog->m_IsOpen && SavefileIO::LoadedSavefile)
        m_Font.AddTextToBatch("Press X to open legend", glm::vec2(m_ScreenLeft + 20, m_ScreenTop - 30), 0.5f);  

    if (SavefileIO::LoadedSavefile)
    {
        if (SavefileIO::GameIsRunning)
        {
            m_Font.AddTextToBatch("BotW is running.", glm::vec2(m_ScreenRight - 20, m_ScreenTop - 30), 0.5f, glm::vec3(1.0f), ALIGN_RIGHT);
            m_Font.AddTextToBatch("Loaded older save", glm::vec2(m_ScreenRight - 20, m_ScreenTop - 60), 0.5f, glm::vec3(1.0f), ALIGN_RIGHT);
        }

        float bottomTextX = m_ScreenRight - 30;

        if (SavefileIO::MasterModeFileExists && !m_LoadMasterMode)
            m_Font.AddTextToBatch("Press Y to load master mode", glm::vec2(bottomTextX, m_ScreenBottom + 55), 0.5f, glm::vec3(1.0f), ALIGN_RIGHT);  
        else if (m_LoadMasterMode)
            m_Font.AddTextToBatch("Press Y to load normal mode", glm::vec2(bottomTextX, m_ScreenBottom + 55), 0.5f, glm::vec3(1.0f), ALIGN_RIGHT);  
    
        m_Font.AddTextToBatch("L and R to zoom, (-) to change user, (+) to exit", 
            glm::vec2(bottomTextX, m_ScreenBottom + 20), 0.5f, glm::vec3(1.0f), ALIGN_RIGHT);  
    }

    m_KorokDialog->Render(m_ProjectionMatrix, m_ViewMatrix);

    glm::mat4 emptyViewMatrix(1.0);
    m_Font.m_ViewMatrix = &emptyViewMatrix; // Don't draw the text relative to the camera 

    m_Font.RenderBatch();

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
    delete[] m_Koroks;
    delete[] m_Shrines;
    delete[] m_Hinoxes;
    delete[] m_Taluses;
    delete[] m_Moldugas;
    delete[] m_Locations;

    delete m_Legend;
    delete m_NoSavefileDialog;
    delete m_GameRunningDialog;
    delete m_MasterModeDialog;
    delete m_KorokDialog;
}

TexturedQuad Map::m_MapBackground;
Font Map::m_Font;
LineRenderer* Map::m_LineRenderer;
TexturedQuad Map::m_MasterModeIcon;

float Map::m_Zoom = Map::m_DefaultZoom;

glm::mat4 Map::m_ProjectionMatrix = glm::mat4(1.0f);
glm::mat4 Map::m_ViewMatrix = glm::mat4(1.0f);

glm::vec2 Map::m_CameraPosition = glm::vec2(0.0f, 0.0f);
glm::vec2 Map::m_PrevCameraPosition;

int Map::m_PrevTouchCount = 0;
glm::vec2 Map::m_PrevTouchPosition;
glm::vec2 Map::m_StartDragPos;
bool Map::m_IsDragging = false;
bool Map::m_ShouldExit = false;
bool Map::m_LoadMasterMode = false;

PadState* Map::m_Pad;
MapObject<Data::Korok>* Map::m_Koroks;
MapObject<Data::Shrine>* Map::m_Shrines;
MapObject<Data::DLCShrine>* Map::m_DLCShrines;
MapObject<Data::Hinox>* Map::m_Hinoxes;
MapObject<Data::Talus>* Map::m_Taluses;
MapObject<Data::Molduga>* Map::m_Moldugas;
MapLocation* Map::m_Locations;

Legend* Map::m_Legend;
KorokDialog* Map::m_KorokDialog;
Dialog* Map::m_NoSavefileDialog;
Dialog* Map::m_GameRunningDialog;
Dialog* Map::m_MasterModeDialog;