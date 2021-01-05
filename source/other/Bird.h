#pragma once

#include <glm/vec2.hpp>

#include "Graphics/Shader.h"
#include "Graphics/Mesh.hpp"

struct NWindow;

class Bird
{
public:
	NWindow *m_Window = nullptr;

	static Texture2D* m_Texture;
    static Shader m_Shader;
    Mesh<TextureVertex>* m_Mesh;

	glm::vec2 m_Position = glm::vec2(0.0f, 0.0f);
	glm::vec2 m_Velocity = glm::vec2(0.0f, 0.0f);

	bool m_ShouldJump = false;

    float m_Rotation = 0.0f;
    float m_Scale = 0.85f;

public: 
	Bird();
	Bird(NWindow* window);

	void Update();
	void Draw();

	int CheckPipeCollision();
	bool CheckGroundCollision();

	~Bird();
};