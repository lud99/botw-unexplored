#pragma once

#include "Graphics/Quad.h"

class IconButton;

class Legend
{
public:
    Legend();

    void Update();
    void Render();

    ~Legend();

public:
    Quad m_Background;

    TexturedQuad m_ShrineIcon;

    std::vector<IconButton*> m_Buttons;

    int m_PrevTouchCount = 0;
};

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

    ~IconButton();

public:
    Quad m_Button;
    TexturedQuad m_Icon;
    std::string m_Text;

    ButtonTypes m_Type = ButtonTypes::Koroks;

    glm::vec2 m_Position;
    float m_Width = 0.0f;
    float m_Height = 0.0f;

    glm::vec4 m_BackgroundColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
};