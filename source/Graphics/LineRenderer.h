#pragma once

#include <glm/vec2.hpp>

#include "Mesh.hpp"
#include "Shader.h"

class LineRenderer
{
public:
    LineRenderer();

    void AddLine(glm::vec2 start, glm::vec2 end, float width = 1.0f, glm::vec4 color = glm::vec4(1.0f));

    void RenderLines(glm::mat4 projMat, glm::mat4 viewMat = glm::mat4(1.0f));

    ~LineRenderer();

public:
    Mesh<ColorVertex> m_Mesh;
    Shader m_Shader; 

    const int MAX_LINES = 300;
};