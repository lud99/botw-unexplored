#include "LineRenderer.h"

#include <iostream>
#include <math.h>

#include "BasicVertices.h"

#define PI 3.14159265358979323846f

LineRenderer::LineRenderer()
{
    // Shader
    std::string vertexShaderSource = R"text(
        #version 330 core

        layout (location = 0) in vec3 position;
        layout (location = 1) in vec4 color;

        out vec4 passColor;

        uniform mat4 u_ProjectionMatrix = mat4(1.0);
        uniform mat4 u_ViewMatrix = mat4(1.0);

        void main()
        {
            gl_Position = u_ProjectionMatrix * u_ViewMatrix * vec4(position, 1.0);

            passColor = color;
        }
    )text";

    std::string fragmentShaderSource = R"text(
        #version 330 core

        layout (location = 0) out vec4 color;

        in vec4 passColor;

        void main()
        {
            color = passColor;

            if (color.a == 0.0)
                discard;
        }
    )text";

    m_Shader = ShaderLoader::CreateShaderFromSource(vertexShaderSource, fragmentShaderSource);

    // Mesh
    m_Mesh.m_UseDynamicBuffer = true;
    m_Mesh.m_DynamicBufferSize = sizeof(ColorVertex) * 4 * MAX_LINES;
    m_Mesh.CreateEmptyBuffer();
}

void LineRenderer::AddLine(glm::vec2 start, glm::vec2 end, float width, glm::vec4 color)
{
    glm::vec2 line = end - start;

    // Weird math for rotating the positions (kind of)
    float angle = 0.5 * PI - atan(abs(line.y / line.x));
    glm::vec2 topLeft(start.x + width * cos(angle), start.y + width * sin(angle));
    glm::vec2 topRight(end.x + width * cos(angle), end.y + width * sin(angle));

    glm::vec2 verticies[4] = { 
        start,
        end,
        topRight,
        topLeft
    };

    for (int i = 0; i < 4; i++)
    {
        ColorVertex vertex;
        vertex.position = glm::vec3(verticies[i], 0.0f);
        vertex.color = color;
        m_Mesh.AddVertex(vertex);
    }

    for (int i = 0; i < 6; i++)
        m_Mesh.AddIndex(BasicVertices::Quad::Indices[i] + m_Mesh.GetVertices().size() - 4);
}

void LineRenderer::RenderLines(glm::mat4 projMat, glm::mat4 viewMat)
{
    m_Shader.Bind();

    m_Shader.SetUniform("u_ProjectionMatrix", projMat);
    m_Shader.SetUniform("u_ViewMatrix", viewMat);

    m_Mesh.Update();
    m_Mesh.Render();

    m_Shader.Unbind();

    m_Mesh.Clear();
}

LineRenderer::~LineRenderer()
{
    m_Shader.Delete();
}