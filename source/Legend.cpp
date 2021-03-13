#include "Legend.h"

#include <iostream>

#include "Map.h"
#include "SavefileIO.h"

Legend::Legend(Map* map)
{
    m_Map = map;

    float left = -m_Map->m_CameraWidth / 2.0f;
    float top = m_Map->m_CameraHeight / 2.0f;
    float bottom = -m_Map->m_CameraHeight / 2.0f;

    float width = 400.0f;

    m_Background.Create(
        glm::vec2(left, bottom),
        glm::vec2(left + width, bottom),
        glm::vec2(left + width, top),
        glm::vec2(left, top)
    );

    m_Background.m_ProjectionMatrix = &m_Map->m_ProjectionMatrix;

    m_Background.m_Color = glm::vec4(0.0f, 0.0f, 0.0f, 0.4f);

    float buttonPadding = 25.0f;
    float buttonVerticalPadding = 25.0f;

    float topOffset = 150.0f;
    float buttonHeight = 65.0f;
    for (int y = 0; y < IconButton::ButtonTypes::Count; y++)
    {
        float topY = top - (buttonHeight * y) - (topOffset + buttonVerticalPadding * y);

        float posLeft = left + buttonPadding;
        float buttonWidth = width - (buttonPadding * 2);

        IconButton* button = new IconButton(m_Map, (IconButton::ButtonTypes)y, glm::vec2(posLeft, topY), buttonWidth, buttonHeight, 1.25f);
        button->m_Button.m_Color = glm::vec4(0.0f, 0.0f, 0.0f, 0.7f);

        m_Buttons.push_back(button);
    }
}

void Legend::Update()
{
    HidTouchScreenState state={0};
    if (hidGetTouchScreenStates(&state, 1))
    {
        // A new touch
        if (state.count != m_PrevTouchCount)
        {   
            m_PrevTouchCount = state.count;

            if (state.count == 1)
            {
                // Convert to more suitable coords
                glm::vec2 touchPosition = glm::vec2(state.touches[0].x - m_Map->m_CameraWidth / 2, -(state.touches[0].y - m_Map->m_CameraHeight / 2)); 

                for (unsigned int i = 0; i < m_Buttons.size(); i++)
                {
                    IconButton* button = m_Buttons[i];

                    float left = button->m_Position.x;
                    float right = button->m_Position.x + button->m_Width;
                    float top = button->m_Position.y;
                    float bottom = button->m_Position.y - button->m_Height;

                    if (touchPosition.x > left && touchPosition.x < right)
                    {
                        if (touchPosition.y > bottom && touchPosition.y < top)
                        {
                            std::cout << "Clicked button!\n";
                        }
                    }
                }
            }
        }
    }
}

void Legend::Render()
{
    m_Background.Render();

    glm::mat4 empty(1.0);
    m_Map->m_Font.m_ViewMatrix = &empty;

    float viewLeft = -m_Map->m_CameraWidth / 2;
    float viewTop = m_Map->m_CameraHeight / 2;
    m_Map->m_Font.RenderText("Map Legend", glm::vec2(viewLeft + 25.0f, viewTop - 55.0f), 0.75f, glm::vec3(1.0));
    m_Map->m_Font.RenderText("X to close", glm::vec2(viewLeft + 400.0f - 25.0f, viewTop - 55.0f), 0.5f, glm::vec3(1.0), ALIGN_RIGHT);

    for (unsigned int i = 0; i < m_Buttons.size(); i++)
    {
        m_Buttons[i]->Render();
    }

    m_Map->m_Font.m_ViewMatrix = &m_Map->m_ViewMatrix;
}

Legend::~Legend()
{
    for (unsigned int i = 0; i < m_Buttons.size(); i++)
        delete m_Buttons[i];
}

IconButton::IconButton()
{

}

IconButton::IconButton(Map* map, ButtonTypes type, glm::vec2 position, float width, float height, float iconScale)
{
    m_Map = map;
    m_Width = width;
    m_Height = height;
    m_Position = position;
    m_Type = type;

    std::string iconPath;
    switch (type)
    {
    case Koroks:
        m_Text = "Koroks";
        iconPath = "romfs:/korokseed.png";

        break;
    case Shrines:
        m_Text = "Shrines";
        iconPath = "romfs:/shrine.png";

        break;
    case Hinoxes:
        m_Text = "Hinoxes";
        iconPath = "romfs:/hinox.png";

        break;
    case Taluses:
        m_Text = "Taluses";
        iconPath = "romfs:/talus.png";

        break;
    case Moldugas:
        m_Text = "Moldugas";
        iconPath = "romfs:/molduga.png";

        break;
    case Locations:
        m_Text = "Locations";
        //iconPath = "romfs:/molduga.png";

        break;
    
    default:
        break;
    }

    m_Button.Create(
        glm::vec2(position.x, position.y + height),
        glm::vec2(position.x + width, position.y + height),
        glm::vec2(position.x + width, position.y),
        glm::vec2(position.x, position.y)
    );

    m_Button.m_ProjectionMatrix = &m_Map->m_ProjectionMatrix;

    m_Icon.Create(iconPath);
    m_Icon.m_Scale = iconScale;

    float iconLeftMargin = 35.0f;
    m_Icon.m_Position = glm::vec2(position.x + iconLeftMargin, position.y + height / 2.0f);

    m_Icon.m_ProjectionMatrix = &m_Map->m_ProjectionMatrix;
}

void IconButton::Render()
{
    m_Button.Render();
    m_Icon.Render();

    float mainTextMargin = 35.0f;
    glm::vec2 mainTextPosition(
        m_Icon.m_Position.x + mainTextMargin, 
        m_Icon.m_Position.y - m_Height / 8.0f
    );

    float countTextMargin = 20.0f;
    glm::vec2 countTextPosition(
        m_Position.x + m_Width - countTextMargin, 
        mainTextPosition.y
    );

    std::string countString = "";
    switch (m_Type)
    {
    case Koroks:
        countString = std::to_string(SavefileIO::foundKoroks.size()) + "/" + std::to_string(Data::KoroksCount);
        break;
    case Shrines:
        countString = "placeholder";
        break;
    case Hinoxes:
        countString = std::to_string(SavefileIO::defeatedHinoxes.size()) + "/" + std::to_string(Data::HinoxesCount);
        break;
    case Taluses:
        countString = std::to_string(SavefileIO::defeatedTaluses.size()) + "/" + std::to_string(Data::TalusesCount);
        break;
    case Moldugas:
        countString = std::to_string(SavefileIO::defeatedMoldugas.size()) + "/" + std::to_string(Data::MoldugasCount);
        break;
    case Locations:
        countString = std::to_string(SavefileIO::visitedLocations.size()) + "/" + std::to_string(Data::LocationsCount);
        break;

    default:
        break;
    }

    m_Map->m_Font.RenderText(m_Text, mainTextPosition, 0.5f, glm::vec3(1.0));
    m_Map->m_Font.RenderText(countString, countTextPosition, 0.5f, glm::vec3(1.0), ALIGN_RIGHT);
}