#include "Background.h"

#include <iostream>

#include <switch/display/native_window.h>

#include "Level.h"

#include "Graphics/BasicVertices.h"
#include "Graphics/Shader.h"

Background::Background()
{

}

Background::Background(NWindow* window)
{
	m_Window = window;

    m_Mesh = new Mesh<TextureVertex>;

	// Only load the texture once
	if (!m_Texture) 
		m_Texture = new Texture2D("romfs:/Images/Backgrounds/BackgroundDay.png");

	// Only load the shader once
    if (m_Shader.m_id == 0) {
        std::string vertexShaderSource = R"text(
            #version 330 core

            layout (location = 0) in vec3 position;
            layout (location = 1) in vec4 color;
            layout (location = 2) in vec2 texCoord;

            out vec2 passTextureCoord;

            uniform mat4 u_ProjectionMatrix = mat4(1.0);
            uniform mat4 u_ModelMatrix = mat4(1.0);

            void main()
            {
                gl_Position = u_ProjectionMatrix * u_ModelMatrix * vec4(position, 1.0);

                passTextureCoord = texCoord;
            }
        )text";

       std::string fragmentShaderSource = R"text(
            #version 330 core

            layout (location = 0) out vec4 color;
            in vec2 passTextureCoord;

            uniform float u_Offset;

            uniform sampler2D tex;

            void main()
            {
                float texturePosition = mod(passTextureCoord.x - u_Offset, 1);

                color = texture(tex, vec2(texturePosition, passTextureCoord.y));

                if (color.a == 0.0)
                    discard;
            }
        )text";

        m_Shader = ShaderLoader::CreateShaderFromSource(vertexShaderSource, fragmentShaderSource);
    }

    m_Mesh->m_Texture = m_Texture;

    glm::vec3 vertexPositions[4];
    BasicVertices::Quad::Construct(vertexPositions, m_Window->width, m_Texture->m_Height);

    float stretchFactor = m_Window->width / m_Texture->m_Width;

    for (int i = 0; i < 4; i++)
    {
        TextureVertex vertex;
        vertex.position = vertexPositions[i];
        vertex.textureCoord = BasicVertices::Quad::TextureCoordinates[i] * glm::vec2(stretchFactor, 1.0f);
        m_Mesh->AddVertex(vertex);
    }

    for (int i = 0; i < 6; i++)
        m_Mesh->AddIndex(BasicVertices::Quad::Indices[i]);

    m_Mesh->Update();

	m_Position = glm::vec2(0, m_Window->height / 2.0f + 100.0f);

    float magicRatio = (6 / 4.71);

	m_ScrollSpeed = glm::vec2(Level::BackgroundScrollSpeed / magicRatio, 0);
}

void Background::Update()
{
	m_ScrollTime += 1.0f / 60.0f;
}

void Background::Draw()
{
    glm::mat4 modelMatrix(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(m_Position.x, m_Position.y, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(m_Scale, m_Scale, 1.0f));

    m_Shader.Bind();

    m_Shader.SetUniform("u_ProjectionMatrix", Level::ProjectionMatrix);
    m_Shader.SetUniform("u_ModelMatrix", modelMatrix);
    m_Shader.SetUniform("u_Offset", m_ScrollTime * m_ScrollSpeed.x);

    m_Mesh->Render();
    
    m_Shader.Unbind();
}

Background::~Background()
{

}

Texture2D* Background::m_Texture;
Shader Background::m_Shader;