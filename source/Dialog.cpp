#include "Dialog.h"

#include "Map.h"
#include "Legend.h"

Dialog::Dialog(glm::vec2 position, float width, float height)
{
    m_Position = position;
    m_Width = width;
    m_Height = height;

    m_Background.Create(position, width, height);
    m_Background.m_Color = glm::vec4(0.0f, 0.0f, 0.0f, 0.7f);

    m_Background.m_ProjectionMatrix = &Map::m_ProjectionMatrix;

    float bgBottom = m_Background.m_Position.y - m_Height / 2.0f;
    float bgLeft = m_Background.m_Position.x - m_Width / 2.0f;
    float bgRight = m_Background.m_Position.x + m_Width / 2.0f;

    float bottomMargin = 40.0f;
    float sideMargin = 60.0f;
    float buttonHeight = 75.0f;
    float button1Width = 150.0f;
    float buttonY = bgBottom + bottomMargin + buttonHeight;
    float buttonX = bgLeft + sideMargin;

    m_ExitButton = new Button(glm::vec2(buttonX, buttonY), button1Width, buttonHeight, "Exit");
    m_ExitButton->m_Button.m_Color = glm::vec4(197, 77, 77, 0.8f);

    float button2Width = 300.0f;
    float button2X = bgRight - sideMargin - button2Width;
    m_ChooseProfileButton = new Button(glm::vec2(button2X, buttonY), button2Width, buttonHeight, "Choose another profile");

    UpdateSelectedButton();
}

bool Dialog::IsPositionOn(glm::vec2 position)
{
    if (position.x > m_Position.x - m_Width / 2.0f && position.x < m_Position.x + m_Width / 2.0f)
    {
        if (position.y > m_Position.x - m_Height / 2.0f && position.y < m_Position.x + m_Height / 2.0f)
            return true;
    }

    return false;
}

void Dialog::UpdateSelectedButton()
{
    m_ExitButton->m_IsSelected = false;
    m_ChooseProfileButton->m_IsSelected = false;

    if (m_SelectedButton == 0)
        m_ExitButton->m_IsSelected = true;
    else
        m_ChooseProfileButton->m_IsSelected = true;
}

void Dialog::Update()
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

                if (m_ExitButton->IsPositionOn(touchPosition)) {
                    m_ExitButton->Click();
                    m_SelectedButton = 0;

                    Map::m_ShouldExit = true;
                } else if (m_ChooseProfileButton->IsPositionOn(touchPosition))
                {
                    m_ChooseProfileButton->Click();
                    m_SelectedButton = 1;

                    Map::m_ShouldChooseProfile = true;
                }
                    
                UpdateSelectedButton();
            }
        }
    }
    
    u64 buttonsPressed = padGetButtonsDown(Map::m_Pad);
    if (buttonsPressed & HidNpadButton_Left)
    {
        if (m_SelectedButton == 1)
            m_SelectedButton = 0;

        UpdateSelectedButton();
    }
    if (buttonsPressed & HidNpadButton_Right)
    {
        if (m_SelectedButton == 0)
            m_SelectedButton = 1;

        UpdateSelectedButton();
    }
    if (buttonsPressed & HidNpadButton_A)
    {
        // Exit button
        if (m_SelectedButton == 0)
        {
            Map::m_ShouldExit = true;
        } 
        else { // Choose profile button
            Map::m_ShouldChooseProfile = true;
        }
    }
}

void Dialog::Render()
{
    m_Background.Render();

    glm::mat4 emptyViewMatrix(1.0f);
    Map::m_Font.m_ViewMatrix = &emptyViewMatrix;

    float top = m_Background.m_Position.y + m_Height / 2.0f;
    float titleTopMargin = 75.0f;

    glm::vec2 titleSize = Map::m_Font.RenderText("No save data found for that user", glm::vec2(0.0f, top - titleTopMargin), 0.75f, glm::vec3(1.0f), ALIGN_CENTER);

    m_ExitButton->Render();
    m_ChooseProfileButton->Render();

    Map::m_Font.m_ViewMatrix = &Map::m_ViewMatrix;
}

void Dialog::SetOpen(bool open)
{
    if (open)
    {
        m_IsOpen = true;
        Map::m_IsLegendOpen = false;
    } else {
        m_IsOpen = false;
    }
}

Dialog::~Dialog()
{

}


Button::Button(glm::vec2 position, float width, float height, const std::string& text)
{
    m_Width = width;
    m_Height = height;
    m_Position = position;
    m_Text = text;

    m_Button.Create(
        glm::vec2(position.x, position.y - height),
        glm::vec2(position.x + width, position.y - height),
        glm::vec2(position.x + width, position.y),
        glm::vec2(position.x, position.y)
    );

    m_Button.m_Color = Button::HighlightedColor;
    m_Button.m_ProjectionMatrix = &Map::m_ProjectionMatrix;

    m_Border.Create(
        "romfs:/buttonborder.png",
        glm::vec2(position.x, position.y - height),
        glm::vec2(position.x + width, position.y - height),
        glm::vec2(position.x + width, position.y),
        glm::vec2(position.x, position.y)
    );
    m_Border.m_ProjectionMatrix = &Map::m_ProjectionMatrix;
}

bool Button::IsPositionOn(glm::vec2 position)
{
    float left = m_Position.x;
    float right = m_Position.x + m_Width;
    float top = m_Position.y;
    float bottom = m_Position.y - m_Height;

    if (position.x > left && position.x < right)
    {
        if (position.y > bottom && position.y < top)
        {
            return true;
        }
    }

    return false;
}

void Button::Render()
{
    m_Button.Render();
    if (m_IsSelected) m_Border.Render();

    float mainTextMargin = 35.0f;
    glm::vec2 mainTextPosition(
        m_Position.x + m_Width / 2,//m_Icon.m_Position.x + mainTextMargin, 
        m_Position.y - (m_Height / 1.5f)
    );

    // float countTextMargin = 20.0f;
    // glm::vec2 countTextPosition(
    //     m_Position.x + m_Width - countTextMargin, 
    //     mainTextPosition.y
    // );

    Map::m_Font.RenderText(m_Text, mainTextPosition, 0.55f, glm::vec3(1.0), ALIGN_CENTER);
    //Map::m_Font.RenderText(countString, countTextPosition, 0.5f, glm::vec3(1.0), ALIGN_RIGHT);
}

bool Button::Click()
{
    m_IsToggled = !m_IsToggled;

    // if (m_IsToggled)
    //     m_Button.m_Color = Button::HighlightedColor;
    // else
    //     m_Button.m_Color = Button::DefaultColor;

    return m_IsToggled;
}

bool Button::Click(bool toggled)
{
    m_IsToggled = toggled;

    // if (m_IsToggled)
    //     m_Button.m_Color = Button::HighlightedColor;
    // else
    //     m_Button.m_Color = Button::DefaultColor;

    return m_IsToggled;
}

constexpr glm::vec4 Button::HighlightedColor;
constexpr glm::vec4 Button::DefaultColor;
constexpr glm::vec4 Button::SelectedColor;