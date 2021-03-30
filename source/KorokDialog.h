#pragma once

#include "Graphics/Quad.h"

class KorokDialog
{
public:
    KorokDialog();

    void Render(glm::mat4 projMat = glm::mat4(1.0f), glm::mat4 viewMat = glm::mat4(1.0f));
    
    void SetSeed(int seed, int korokIndex);
    void SetOpen(bool open);

    void SetPosition(glm::vec2 position);
    glm::vec2 GetPosition();

    ~KorokDialog();

public:
    const int Width = 350.0f;
    const int m_Margin = 30.0f;

    Quad m_Background;
    TexturedQuad* m_Image = nullptr;

    std::string m_Text;
    int m_Seed = -1;
    int m_KorokIndex = -1;

    bool m_IsOpen = false;
};