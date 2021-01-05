#include "Bird.h"

#include <vector>
#include <iostream>

#include <switch/display/native_window.h>

#include "Level.h"
#include "Ground.h"
#include "PipePair.h"

#include "Graphics/BasicVertices.h"
#include "Graphics/Shader.h"

Bird::Bird()
{

}

Bird::Bird(NWindow* window)
{
	m_Window = window;

    m_Mesh = new Mesh<TextureVertex>;

	// Only load the texture once
	if (!m_Texture) 
		m_Texture = new Texture2D("romfs:/Images/Bird/BirdUpflap.png");

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

	m_Position = glm::vec2(m_Window->width / 2.0f, m_Window->height / 2.0f + 100.0f);
}

void Bird::Update()
{
	int pipeCollision = CheckPipeCollision(); // -1: top, 0: none, 1: bottom
	bool groundCollision = CheckGroundCollision();

	// If touching a bottom pipe for the first time
	if (pipeCollision == 1 && !Level::IsGameOver)
		m_Velocity.y = 12.5f; // Small jump

	// If touching a pipe or the ground
	if (pipeCollision != 0 || groundCollision)
		Level::IsGameOver = true;

	if (m_ShouldJump && !Level::IsGameOver)
		m_Velocity.y = 13.0f;
	else
		m_Velocity.y -= 1.0f;

	if (!groundCollision)
	{
	    m_Position.y += m_Velocity.y;
        m_Rotation = m_Velocity.y * 1.0f;
	}

	m_ShouldJump = false;
}

void Bird::Draw()
{
    glm::mat4 modelMatrix(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(m_Position.x, m_Position.y, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(m_Scale, m_Scale, 1.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(m_Rotation), glm::vec3(0.0f, 0.0f, 1.0f));

    m_Shader.Bind();

    m_Shader.SetUniform("u_ProjectionMatrix", Level::ProjectionMatrix);
    m_Shader.SetUniform("u_ModelMatrix", modelMatrix);

    m_Mesh->Render();
    
    m_Shader.Unbind();
}

int Bird::CheckPipeCollision()
{
	std::vector<PipePair*>& pipes = Level::GetPipePairs();

	// Slithtly smaller hitbox
	glm::vec2 size(m_Texture->m_Width * 0.75f, m_Texture->m_Height * 0.75f);

	for (unsigned int i = 0; i < pipes.size(); i++)
	{
		Pipe* topPipe = pipes[i]->GetTopPipe();
		Pipe* bottomPipe = pipes[i]->GetBottomPipe();

		float left = m_Position.x - size.x / 2.0;
		float right = m_Position.x + size.x / 2.0f;
		float top = m_Position.y + size.y / 2.0f;
		float bottom = m_Position.y - size.y / 2.0f;

		float pipeLeft = pipes[i]->GetX() - Pipe::m_Width / 2;
		float pipeRight = pipes[i]->GetX() + Pipe::m_Width / 2;
		float topPipeBottom = topPipe->m_Position.y - Pipe::m_Height / 2;
		float bottomPipeTop = bottomPipe->m_Position.y + Pipe::m_Height / 2;

		// Horizontal collision
		if (right > pipeLeft && left < pipeRight)
		{
			// Vertical collision
			if (bottom < bottomPipeTop || top > topPipeBottom) {

				//Determine if collision was a top or bottom collision by calculating with pipe was the closest
				return (abs(top - topPipeBottom) > abs(bottom - bottomPipeTop) ? 1 : -1);
			}
		}
	}

	return 0;
}


bool Bird::CheckGroundCollision()
{
	glm::vec2 size(m_Texture->m_Width * m_Scale, m_Texture->m_Height * m_Scale);

	float bottom = m_Position.y - size.y / 2.0f;
	float groundTop = Level::GetGround()->m_Height / 2;

	if (bottom < groundTop) 
		return true;
	else
		return false;
}

Bird::~Bird()
{
    printf("Bird destructor");

    m_Shader.Delete();
    
    delete m_Texture;
}

Texture2D* Bird::m_Texture;
Shader Bird::m_Shader;