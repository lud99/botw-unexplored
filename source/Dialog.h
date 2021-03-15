#pragma once

#include "Graphics/Quad.h"

#include <string>

class Button
{
public:
    Button();

    // Position is top-left corner
    Button(glm::vec2 position, float width, float height, const std::string& text);

    bool IsPositionOn(glm::vec2 position);

    void Render();

    bool Click();
    bool Click(bool toggled);

    ~Button();

public:
    Quad m_Button;
    TexturedQuad m_Border;

    std::string m_Text;

    bool m_IsSelected = false;
    bool m_IsToggled = false;

    static constexpr glm::vec4 HighlightedColor = glm::vec4(64.0f, 113.0f, 145.0f, 0.8f);
    static constexpr glm::vec4 DefaultColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.9f);
    static constexpr glm::vec4 SelectedColor = glm::vec4(85.0f, 158.0f, 100.0f, 0.6f);

    // Origin is top-left corner
    glm::vec2 m_Position;
    float m_Width = 0.0f;
    float m_Height = 0.0f;

    glm::vec4 m_BackgroundColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
};

class Dialog
{
public:
    Dialog(glm::vec2 position, float width, float height);

    bool IsPositionOn(glm::vec2 position);
    void UpdateSelectedButton();

    void Update();
    void Render();

    void SetOpen(bool open);

    ~Dialog();

public:
    Quad m_Background;

    Button* m_ChooseProfileButton;
    Button* m_ExitButton;

    int m_PrevTouchCount = 0;
    unsigned int m_SelectedButton = 1;

    bool m_IsOpen = false;

    glm::vec2 m_Position = glm::vec2(0.0f, 0.0f);

    float m_Width = 0.0f;
    float m_Height = 0.0f;
};