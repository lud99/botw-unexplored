#pragma once

#include "Graphics/Quad.h"

class KorokDialog
{
public:
    KorokDialog();

    void Render(glm::mat4 projMat = glm::mat4(1.0f), glm::mat4 viewMat = glm::mat4(1.0f));
    
    void SetSeed(int seed);
    void SetOpen(bool open);

    void SetPosition(glm::vec2 position);
    glm::vec2 GetPosition();

    ~KorokDialog();

public:
    const int Width = 400.0f;
    const int Height = 400.0f;

    Quad m_Background;
    TexturedQuad* m_Image;
    
    std::string m_Text;

    bool m_IsOpen = false;
};