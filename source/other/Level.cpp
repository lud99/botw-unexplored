#include "Level.h"

#include <switch/display/native_window.h>

#include <iostream>
#include <vector>
#include <math.h>

#include "Bird.h"
#include "Background.h"
#include "Ground.h"
#include "PipePair.h"

namespace Level
{
	namespace
	{
		NWindow* m_Window;

		Bird* m_Bird;
		Background* m_Background;
		Ground* m_Ground;
        Ground* m_Ground2;
		
        std::vector<PipePair*> m_PipePairs;

		float m_ElapsedTime = 0;
        s32 m_PreviousTouchCount = 0;
	}

	int PipePairCount = 0;

	bool IsGameOver = false;
    PadState m_Pad;

    glm::mat4 ProjectionMatrix = glm::mat4(1.0);

	void Init(NWindow* window)
	{
		m_Window = window;

		IsGameOver = false;

        float width = m_Window->width;
        float height = m_Window->height;

        ProjectionMatrix = glm::ortho(0.0f, width, 0.0f, height, -1.0f, 1.0f);

		PipePairCount = (int)ceil((m_Window->width / (Pipe::m_Width + PipeHorizontalSpace)) + 1);

		// Remove any pipes
		m_PipePairs.clear();

		for (int i = 0; i < PipePairCount; i++)
			m_PipePairs.push_back(new PipePair(window));

		m_Bird = new Bird(window);
		m_Background = new Background(window);
		m_Ground = new Ground(window);
        m_Ground2 = new Ground(window);
        m_Ground2->m_Position.x = m_Ground->m_Position.x + Ground::m_Width * 2.25f;

		// Runs for every pair of pipes
		for (int i = 0; i < PipePairCount; i++) {
			PipePair* pipePair = m_PipePairs[i];

			float x = window->width + StartMargin + ((Pipe::m_Width + PipeHorizontalSpace) * i);

			PipePair::Positions pipePositions = pipePair->GenerateY();

			pipePair->SetX(x);
			pipePair->GetBottomPipe()->m_Position.y = pipePositions.bottomY;
			pipePair->GetTopPipe()->m_Position.y = pipePositions.topY;
		}
	}

    void ProcessInput(PadState& pad)
    {
        m_Pad = pad;

        u32 kDown = padGetButtonsDown(&pad);

        // B button pressed
        if (kDown & HidNpadButton_B)
            m_Bird->m_ShouldJump = true;

        // Touch 
        HidTouchScreenState state={0};
        if (hidGetTouchScreenStates(&state, 1)) {
            // Only detect touches the frame they happen
            if (state.count != m_PreviousTouchCount)
            {
                m_PreviousTouchCount = state.count;

                // Don't jump when a finger is released
                if (state.count == 1)
                    m_Bird->m_ShouldJump = true;
            }
        }

        if (m_Bird->m_ShouldJump && IsGameOver)
        {
            IsGameOver = false;
            Init(m_Window);

            m_Bird->m_ShouldJump = true;
        }
    }

	void Update()
	{
		m_ElapsedTime += 1.0f / 60.0f;

		m_Bird->Update();

		// Only update the bird if gameover, not the rest
		if (IsGameOver) return;

		m_Background->Update();

        // Ground update
        m_Ground->Update();
        if (m_Ground->m_Position.x + Ground::m_Width * 2.0f <= 0) 
            m_Ground->m_Position.x = m_Window->width + Ground::m_Width * 2.0f;

        m_Ground2->Update();
        if (m_Ground2->m_Position.x + Ground::m_Width * 2.0f <= 0) 
            m_Ground2->m_Position.x = m_Window->width + Ground::m_Width * 2.0f;

		for (unsigned int i = 0; i < m_PipePairs.size(); i++)
			m_PipePairs[i]->Update();
	}

	void Draw()
	{
		m_Background->Draw();

		for (unsigned int i = 0; i < m_PipePairs.size(); i++)
			m_PipePairs[i]->Draw();

        m_Bird->Draw();
        m_Ground->Draw();
        m_Ground2->Draw();
	}

    void Exit()
    {
        delete m_Bird;
    }

	std::vector<PipePair*>& GetPipePairs() { return m_PipePairs; }
	Ground* GetGround() { return m_Ground; }

	NWindow* GetWindow() { return m_Window; }
}