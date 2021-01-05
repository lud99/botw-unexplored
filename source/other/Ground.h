#pragma once

#include <glm/vec2.hpp>

#include "Graphics/Shader.h"
#include "Graphics/Mesh.hpp"

struct NWindow;

class Ground
{
public:
	NWindow* m_Window = nullptr;

	static Texture2D* m_Texture;
    static Shader m_Shader;
    Mesh<TextureVertex>* m_Mesh;

	glm::vec2 m_Position;

	glm::vec2 m_ScrollSpeed;
	float m_ScrollTime = 0;

    static constexpr float m_Scale = 2.0f;

    static const int m_TextureWidth = 336;
    static const int m_TextureHeight = 112;

	static constexpr float m_Width = m_TextureWidth * m_Scale;
    static constexpr float m_Height = m_TextureHeight * m_Scale;

public:
	Ground();
	Ground(NWindow* window);

	void Update();
	void Draw();

	glm::vec2 GetSize();

	~Ground();

private:
	void CenterOrigin();
};