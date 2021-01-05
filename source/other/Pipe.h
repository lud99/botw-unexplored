#pragma once

#include <glm/vec2.hpp>

#include "Graphics/Shader.h"
#include "Graphics/Mesh.hpp"

struct NWindow;

class Pipe
{
public:
	NWindow *m_Window = nullptr;

	static Texture2D* m_Texture;
    static Shader m_Shader;
    Mesh<TextureVertex>* m_Mesh;

	glm::vec2 m_Position = glm::vec2(0.0f, 0.0f);

	float m_Speed = 0.0f;

	glm::vec2 m_Scale = glm::vec2(0.75f, 0.75f);

    static const int m_TextureWidth = 138;
    static const int m_TextureHeight = 793;

	static constexpr float m_Width = m_TextureWidth * 0.75f;
    static constexpr float m_Height = m_TextureHeight * 0.75f;

	static const int NoFlip = 1, Flip = -1;

public:
    Pipe();
	Pipe(NWindow* window);
    
	void Update();
	void Draw();

	void SetFlip(int flip);

    ~Pipe();
};