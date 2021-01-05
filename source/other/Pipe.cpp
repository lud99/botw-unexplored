#include "Pipe.h"

#include <switch/display/native_window.h>

#include "Level.h"

#include "Graphics/BasicVertices.h"
#include "Graphics/Shader.h"

Pipe::Pipe()
{

}

Pipe::Pipe(NWindow* window)
{
	m_Window = window;

    m_Mesh = new Mesh<TextureVertex>;

	// Only load the texture once
	if (!m_Texture) 
		m_Texture = new Texture2D("romfs:/Images/Pipe.png");

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
        
    m_Mesh->m_Texture = m_Texture;

    glm::vec3 vertexPositions[4];
    BasicVertices::Quad::Construct(vertexPositions, m_Texture->m_Width, m_Texture->m_Height);

    for (int i = 0; i < 4; i++)
    {
        TextureVertex vertex;
        vertex.position = vertexPositions[i];
        vertex.textureCoord = BasicVertices::Quad::TextureCoordinates[i];
        m_Mesh->AddVertex(vertex);
    }

    for (int i = 0; i < 6; i++)
        m_Mesh->AddIndex(BasicVertices::Quad::Indices[i]);

    m_Mesh->Update();
    
	//m_Sprite.setOrigin(Vector2f(0.0f, 0.0f));

	m_Speed = Level::ScrollSpeed;// * 2.43478f * 4.0f * (1.0f / 0.75f);//m_Texture->m_Width * 0.09f;// * (138 * 0.75f / 9.5f);
}

void Pipe::Update()
{
	m_Position.x += m_Speed;
}

void Pipe::Draw()
{
	glm::mat4 modelMatrix(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(m_Position.x, m_Position.y, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(m_Scale.x, m_Scale.y, 1.0f));

    m_Shader.Bind();

    m_Shader.SetUniform("u_ProjectionMatrix", Level::ProjectionMatrix);
    m_Shader.SetUniform("u_ModelMatrix", modelMatrix);

    m_Mesh->Render();
    
    m_Shader.Unbind();
}

void Pipe::SetFlip(int flip)
{
    m_Scale.y *= flip;
}

Pipe::~Pipe()
{

}

Texture2D* Pipe::m_Texture;
Shader Pipe::m_Shader;