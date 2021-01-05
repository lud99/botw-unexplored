#include "MapLocation.h"

#include <switch.h>

#include "Map.h"
#include "Graphics/BasicVertices.h"
#include "SavefileIO.h"

MapLocation::MapLocation()
{

}

void MapLocation::Init()
{
    // Set the fonts matrices
    m_Font->m_ProjectionMatrix = &m_Map->m_ProjectionMatrix;
    m_Font->m_ViewMatrix = &m_Map->m_ViewMatrix;

    // Set text font
    m_Text.m_Font = m_Font;

    // Create text mesh
    //m_Text.Create(m_LocationData->displayName);
}

void MapLocation::Update()
{
    m_Scale = 0.25f / m_Map->m_Zoom;
}

void MapLocation::Render()
{
    if (m_Found) return;

    m_Font->RenderText(m_LocationData->displayName, m_Position, m_Scale, m_Color);
}

void MapLocation::SetFontMatrices()
{
    m_Font->m_ProjectionMatrix = &m_Map->m_ProjectionMatrix;
    m_Font->m_ViewMatrix = &m_Map->m_ViewMatrix;
}

MapLocation::~MapLocation()
{

}

Font* MapLocation::m_Font;
Map* MapLocation::m_Map;