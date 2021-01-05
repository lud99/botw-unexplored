#pragma once

#include <glm/vec2.hpp>

#include "Graphics/Shader.h"
#include "Graphics/Mesh.hpp"

struct NWindow;

class Background
{
public:
	NWindow *m_Window = nullptr;

	static Texture2D* m_Texture;
    static Shader m_Shader;
    Mesh<TextureVertex>* m_Mesh;

	glm::vec2 m_Position;

	glm::vec2 m_ScrollSpeed;
	float m_ScrollTime = 0;
    float m_Scale = 2.0f;

public:
	Background();
	Background(NWindow* window);

	void Update();
	void Draw();

	~Background();
};