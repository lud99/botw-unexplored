#pragma once

#include "Graphics/Quad.h"

class Legend;

class IconButton
{
public:
    enum ButtonTypes {
        Koroks = 0,
        Shrines = 1,
        Hinoxes = 2,
        Taluses = 3,
        Moldugas = 4,
        Locations = 5,
        Count = 6
    };

public:
    IconButton();

    // Position is top-left corner
    IconButton(ButtonTypes type, glm::vec2 position, float width, float height, float iconScale = 1.0f);

    void Render();

    bool Click(Legend* legend);
    bool Click(Legend* legend, bool toggled);

    ~IconButton();

public:
    Quad m_Button;
    TexturedQuad m_Border;
    TexturedQuad m_Icon;
    std::string m_Text;

    bool m_IsSelected = false;
    bool m_IsToggled = false;

    ButtonTypes m_Type = ButtonTypes::Koroks;

    static constexpr glm::vec4 HighlightedColor = glm::vec4(64.0f, 113.0f, 145.0f, 0.8f);
    static constexpr glm::vec4 DefaultColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.9f);
    static constexpr glm::vec4 SelectedColor = glm::vec4(85.0f, 158.0f, 100.0f, 0.6f);

    glm::vec2 m_Position;
    float m_Width = 0.0f;
    float m_Height = 0.0f;

    glm::vec4 m_BackgroundColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
};

class Legend
{
public:
    Legend();

    void Update();
    void Render();

    bool IsPositionOnLegend(glm::vec2 position);
    void UpdateSelectedButton();

    ~Legend();

public:
    Quad m_Background;

    std::vector<IconButton*> m_Buttons;

    float m_Width = 350.0f;

    int m_PrevTouchCount = 0;

    int m_HighlightedButton = 0;

    bool m_ShowAll = true;
    bool m_ShowNone = false;

    bool m_Show[IconButton::ButtonTypes::Count];
};