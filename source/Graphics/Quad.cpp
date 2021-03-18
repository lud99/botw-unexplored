#include "Quad.h"

#include "BasicVertices.h"
#include "Texture2D.h"

Quad::Quad()
{

}

void Quad::Create(glm::vec2 position, float width, float height)
{
    glm::vec3 positions[4];
    BasicVertices::Quad::Construct(positions, width, height);

    m_Position = position;

    Create(positions[0], positions[1], positions[2], positions[3]);
}

void Quad::Create(glm::vec2 bottomLeft, glm::vec2 bottomRight, glm::vec2 topRight, glm::vec2 topLeft)
{
    if (m_Shader.m_id == 0) {
        std::string vertexShaderSource = R"text(
            #version 330 core

            layout (location = 0) in vec3 position;

            uniform mat4 u_ProjectionMatrix = mat4(1.0);
            uniform mat4 u_ViewMatrix = mat4(1.0);
            uniform mat4 u_ModelMatrix = mat4(1.0);

            void main()
            {
                gl_Position = u_ProjectionMatrix * u_ViewMatrix * u_ModelMatrix * vec4(position, 1.0);
            }
        )text";

        std::string fragmentShaderSource = R"text(
            #version 330 core

            layout (location = 0) out vec4 color;

            uniform vec4 u_Color;

            void main()
            {
                color = u_Color;

                if (color.a == 0.0)
                    discard;
            }
        )text";

        m_Shader = ShaderLoader::CreateShaderFromSource(vertexShaderSource, fragmentShaderSource);
    }

    m_Corners[0] = bottomLeft;
    m_Corners[1] = bottomRight;
    m_Corners[2] = topRight;
    m_Corners[3] = topLeft;

    m_Mesh.AddVertex(BasicVertex(bottomLeft));
    m_Mesh.AddVertex(BasicVertex(bottomRight));
    m_Mesh.AddVertex(BasicVertex(topRight));
    m_Mesh.AddVertex(BasicVertex(topLeft));

    for (int i = 0; i < 6; i++)
        m_Mesh.AddIndex(BasicVertices::Quad::Indices[i]);

    m_Mesh.Update();
}

void Quad::Render()
{
    m_Shader.Bind();

    if (m_ProjectionMatrix != nullptr) m_Shader.SetUniform("u_ProjectionMatrix", *m_ProjectionMatrix);
    if (m_ViewMatrix != nullptr) m_Shader.SetUniform("u_ViewMatrix", *m_ViewMatrix);

    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(m_Position, 0.0f));

    m_Shader.SetUniform("u_ModelMatrix", modelMatrix);

    glm::vec4 color = m_Color;
    if (color.r > 1.0f) color = color / glm::vec4(255.0f, 255.0f, 255.0f, 1.0f);

    m_Shader.SetUniform("u_Color", color);

    m_Mesh.Render();

    m_Shader.Unbind();
}

Quad::~Quad()
{
    m_Shader.Delete();
}

Shader Quad::m_Shader;

TexturedQuad::TexturedQuad()
{

}

void TexturedQuad::Create(const std::string& texturePath)
{
    m_Texture = new Texture2D(texturePath);

    glm::vec3 positions[4];
    BasicVertices::Quad::Construct(positions, m_Texture->m_Width, m_Texture->m_Height);

    Create(texturePath, positions[0], positions[1], positions[2], positions[3]);
}

void TexturedQuad::Create(const std::string& texturePath, glm::vec2 bottomLeft, glm::vec2 bottomRight, glm::vec2 topRight, glm::vec2 topLeft)
{
    if (!m_Texture)
        m_Texture = new Texture2D(texturePath);

    if (m_Shader.m_id == 0) {
        std::string vertexShaderSource = R"text(
            #version 330 core

            layout (location = 0) in vec3 position;
            layout (location = 1) in vec4 color;
            layout (location = 2) in vec2 texCoord;

            out vec2 passTextureCoord;

            uniform mat4 u_ProjectionMatrix = mat4(1.0);
            uniform mat4 u_ViewMatrix = mat4(1.0);
            uniform mat4 u_ModelMatrix = mat4(1.0);
            uniform float u_Scale;

            void main()
            {
                gl_Position = u_ProjectionMatrix * u_ViewMatrix * u_ModelMatrix * vec4(position * u_Scale, 1.0);

                passTextureCoord = texCoord;
            }
        )text";

        std::string fragmentShaderSource = R"text(
            #version 330 core

            layout (location = 0) out vec4 color;
            in vec2 passTextureCoord;

            uniform sampler2D tex;

            void main()
            {
                color = texture(tex, passTextureCoord);

                if (color.a == 0.0)
                    discard;
            }
        )text";

        m_Shader = ShaderLoader::CreateShaderFromSource(vertexShaderSource, fragmentShaderSource);
    }

    m_Corners[0] = bottomLeft;
    m_Corners[1] = bottomRight;
    m_Corners[2] = topRight;
    m_Corners[3] = topLeft;

    const glm::vec2* texCoords = BasicVertices::Quad::TextureCoordinates;

    m_Mesh.AddVertex(TextureVertex(bottomLeft, texCoords[0]));
    m_Mesh.AddVertex(TextureVertex(bottomRight, texCoords[1]));
    m_Mesh.AddVertex(TextureVertex(topRight, texCoords[2]));
    m_Mesh.AddVertex(TextureVertex(topLeft, texCoords[3]));

    for (int i = 0; i < 6; i++)
        m_Mesh.AddIndex(BasicVertices::Quad::Indices[i]);

    m_Mesh.m_Texture = m_Texture;

    m_Mesh.Update();
}

void TexturedQuad::Render()
{
    m_Shader.Bind();

    if (m_ProjectionMatrix != nullptr) m_Shader.SetUniform("u_ProjectionMatrix", *m_ProjectionMatrix);
    if (m_ViewMatrix != nullptr) m_Shader.SetUniform("u_ViewMatrix", *m_ViewMatrix);

    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(m_Position, 0.0f));

    m_Shader.SetUniform("u_ModelMatrix", modelMatrix);
    m_Shader.SetUniform("u_Scale", m_Scale);

    m_Mesh.Render();

    m_Shader.Unbind();
}

TexturedQuad::~TexturedQuad()
{
    m_Shader.Delete();
    delete m_Texture;
}

Shader TexturedQuad::m_Shader;