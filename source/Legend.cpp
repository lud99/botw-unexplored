#include "Legend.h"

#include <iostream>

#include <glm/gtc/matrix_inverse.hpp>

#include "Map.h"
#include "SavefileIO.h"

Legend::Legend()
{
    float left = -Map::m_CameraWidth / 2.0f;
    float top = Map::m_CameraHeight / 2.0f;
    float bottom = -Map::m_CameraHeight / 2.0f;

    m_Background.Create(
        glm::vec2(left, bottom),
        glm::vec2(left + m_Width, bottom),
        glm::vec2(left + m_Width, top),
        glm::vec2(left, top)
    );

    m_Background.m_ProjectionMatrix = &Map::m_ProjectionMatrix;

    m_Background.m_Color = glm::vec4(0.0f, 0.0f, 0.0f, 0.5f);

    float buttonPadding = 25.0f;
    float buttonVerticalPadding = 25.0f;

    float topOffset = 80.0f;
    float buttonHeight = 65.0f;
    for (int i = 0; i < IconButton::ButtonTypes::Count; i++)
    {
        float topY = top - (buttonHeight * i) - (topOffset + buttonVerticalPadding * i);

        float posLeft = left + buttonPadding;
        float buttonWidth = m_Width - (buttonPadding * 2);

        IconButton* button = new IconButton((IconButton::ButtonTypes)i, glm::vec2(posLeft, topY), buttonWidth, buttonHeight, 1.25f);
        button->m_Button.m_Color = IconButton::DefaultColor;

        m_Buttons.push_back(button);

        m_Show[i] = false;
    }

    // Show koroks and shrines by default
    m_Buttons[0]->Click(this, true);
    m_Show[0] = true;
    m_Buttons[1]->Click(this, true);
    m_Show[1] = true;

    // Set first highlighted button
    UpdateSelectedButton();
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
                glm::vec2 touchPosition = glm::vec2(state.touches[0].x - Map::m_CameraWidth / 2, -(state.touches[0].y - Map::m_CameraHeight / 2)); 

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
                            button->Click(this);

                            m_HighlightedButton = i;
                            UpdateSelectedButton();
                            
                            break;
                        }
                    }
                }
            }
        }
    }

    u64 buttonsPressed = padGetButtonsDown(Map::m_Pad);
    if (buttonsPressed & HidNpadButton_Down)
    {
        if (m_HighlightedButton < (int)m_Buttons.size() - 1)
        {
            m_HighlightedButton++;

            UpdateSelectedButton();
        }
    }
    if (buttonsPressed & HidNpadButton_Up)
    {
        if (m_HighlightedButton > 0)
        {
            m_HighlightedButton--;

            UpdateSelectedButton();
        }
    }
    if (buttonsPressed & HidNpadButton_A)
    {
        // Bounds check
        if (m_HighlightedButton >= 0 && m_HighlightedButton < (int)m_Buttons.size())
            m_Buttons[m_HighlightedButton]->Click(this);
    }
}

void Legend::UpdateSelectedButton()
{
    for (unsigned int i = 0; i < m_Buttons.size(); i++) {
        if (m_Buttons[i]->m_Button.m_Color == IconButton::SelectedColor)
            m_Buttons[i]->m_Button.m_Color = IconButton::DefaultColor;

        m_Buttons[i]->m_IsSelected = false;
    }

    m_Buttons[m_HighlightedButton]->m_IsSelected = true;
}

void Legend::Render()
{
    m_Background.Render();

    glm::mat4 empty(1.0);
    Map::m_Font.m_ViewMatrix = &empty;

    float viewLeft = -Map::m_CameraWidth / 2;
    float viewTop = Map::m_CameraHeight / 2;
    Map::m_Font.RenderText("Legend", glm::vec2(viewLeft + 25.0f, viewTop - 55.0f), 0.75f, glm::vec3(1.0));
    Map::m_Font.RenderText("X to close", glm::vec2(viewLeft + m_Width - 26.0f, viewTop - 55.0f), 0.5f, glm::vec3(1.0), ALIGN_RIGHT);

    for (unsigned int i = 0; i < m_Buttons.size(); i++)
    {
        m_Buttons[i]->Render();
    }

    Map::m_Font.m_ViewMatrix = &Map::m_ViewMatrix;
}

bool Legend::IsPositionOnLegend(glm::vec2 position)
{
    float left = -Map::m_CameraWidth / 2;
    if (position.x > left && position.x < left + m_Width) {
        if (position.y > -Map::m_CameraHeight / 2 && position.y < Map::m_CameraWidth / 2) {
            return true;
        }
    }

    return false;
}

Legend::~Legend()
{
    for (unsigned int i = 0; i < m_Buttons.size(); i++)
        delete m_Buttons[i];
}

IconButton::IconButton()
{

}

IconButton::IconButton(ButtonTypes type, glm::vec2 position, float width, float height, float iconScale)
{
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
        glm::vec2(position.x, position.y - height),
        glm::vec2(position.x + width, position.y - height),
        glm::vec2(position.x + width, position.y),
        glm::vec2(position.x, position.y)
    );

    m_Button.m_ProjectionMatrix = &Map::m_ProjectionMatrix;

    m_Border.Create(
        "romfs:/buttonborder.png",
        glm::vec2(position.x, position.y - height),
        glm::vec2(position.x + width, position.y - height),
        glm::vec2(position.x + width, position.y),
        glm::vec2(position.x, position.y)
    );
    m_Border.m_ProjectionMatrix = &Map::m_ProjectionMatrix;

    m_Icon.Create(iconPath);
    m_Icon.m_Scale = iconScale;

    float iconLeftMargin = 35.0f;
    m_Icon.m_Position = glm::vec2(position.x + iconLeftMargin, position.y - height / 2.0f);

    m_Icon.m_ProjectionMatrix = &Map::m_ProjectionMatrix;
}

void IconButton::Render()
{
    m_Button.Render();
    if (m_IsSelected) m_Border.Render();

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
        if (!SavefileIO::HasDLC)
            countString = std::to_string(SavefileIO::foundShrines.size()) + "/" + std::to_string(Data::ShrineCount);
        else
            countString = std::to_string(SavefileIO::foundShrines.size() + SavefileIO::foundDLCShrines.size()) + "/" + 
                          std::to_string(Data::ShrineCount + Data::DLCShrineCount);
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

    Map::m_Font.RenderText(m_Text, mainTextPosition, 0.5f, glm::vec3(1.0));
    Map::m_Font.RenderText(countString, countTextPosition, 0.5f, glm::vec3(1.0), ALIGN_RIGHT);
}

bool IconButton::Click(Legend* legend)
{
    m_IsToggled = !m_IsToggled;

    if (m_IsToggled)
        m_Button.m_Color = IconButton::HighlightedColor;
    else
        m_Button.m_Color = IconButton::DefaultColor;

    legend->m_Show[m_Type] = m_IsToggled;

    return m_IsToggled;
}

bool IconButton::Click(Legend* legend, bool toggled)
{
    m_IsToggled = toggled;

    if (m_IsToggled)
        m_Button.m_Color = IconButton::HighlightedColor;
    else
        m_Button.m_Color = IconButton::DefaultColor;

    legend->m_Show[m_Type] = m_IsToggled;

    return m_IsToggled;
}

IconButton::~IconButton()
{
    
}

constexpr glm::vec4 IconButton::HighlightedColor;
constexpr glm::vec4 IconButton::DefaultColor;
constexpr glm::vec4 IconButton::SelectedColor;