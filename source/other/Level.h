#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

#include <switch.h>

#include <vector>

struct NWindow;
class Ground;
class PipePair;

namespace Level
{
    extern glm::mat4 ProjectionMatrix;

	const float ScrollSpeed = -5.0f;
	const float BackgroundScrollSpeed = ScrollSpeed / 11.0f;
	const float PipeHorizontalSpace = 200.0f;
	const float PipeVerticalSpace = 200.0f;
    const float StartMargin = 100.0f;

	const float PipeVerticalGroundOffset = 60.0f;
	const float PipeVerticalTopOffset = 60.0f;

	extern bool IsGameOver;

    extern PadState m_Pad;

	void Init(NWindow* window);
    void ProcessInput(PadState& pad);
	void Update();
	void Draw();

    void Exit();

	std::vector<PipePair*>& GetPipePairs();

	Ground* GetGround();

	NWindow* GetWindow();
}