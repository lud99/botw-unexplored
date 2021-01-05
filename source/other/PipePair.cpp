#include <time.h>

#include "PipePair.h"

#include <switch/display/native_window.h>

#include "Level.h"
#include "Ground.h"

PipePair::PipePair()
{

}

PipePair::PipePair(NWindow* window)
{
	m_Window = window;

	m_BottomPipe = new Pipe(window);
	m_TopPipe = new Pipe(window);

	m_BottomPipe->SetFlip(Pipe::NoFlip);
	m_TopPipe->SetFlip(Pipe::Flip);
}

void PipePair::Update()
{
	m_BottomPipe->Update();
	m_TopPipe->Update();

	float rightEdgeX = GetX() + Pipe::m_Width / 2.0f;

	// Check if the pipe is outside the left side of the window
	if (rightEdgeX < 0.0f)
	{
		std::vector<PipePair*> &pipePairs = Level::GetPipePairs();
		PipePair *lastPipePair = pipePairs.back();

		float newX = lastPipePair->GetX() + Pipe::m_Width + Level::PipeHorizontalSpace;

		SetX(newX);

		Positions pipePositions = GenerateY();

		m_BottomPipe->m_Position.y = pipePositions.bottomY;
		m_TopPipe->m_Position.y = pipePositions.topY;
		
		pipePairs.push_back(this);
		pipePairs.erase(pipePairs.begin());

        pipePairs[0]->SetX(pipePairs[0]->GetX() - 6.0f);
	}
}

void PipePair::Draw()
{
	m_BottomPipe->Draw();
	m_TopPipe->Draw();
}

PipePair::Positions PipePair::GenerateY()
{
	float groundHeight = Ground::m_Height / 2.0f;

	// Down is 0, top is positive (window->height)

	float pipeMinY = -Pipe::m_Height / 2.0f + groundHeight + Level::PipeVerticalGroundOffset; 
	float pipeMaxY = m_Window->height - Pipe::m_Height / 2.0f - Level::PipeVerticalTopOffset - Level::PipeVerticalSpace;

	float bottomPipeY = PipePair::RandomY((int)pipeMinY, (int)pipeMaxY);

	float topPipeY = bottomPipeY + Pipe::m_Height + Level::PipeVerticalSpace;

	return Positions(bottomPipeY, topPipeY);
}

void PipePair::SetX(float x) 
{
	m_BottomPipe->m_Position.x = x;
	m_TopPipe->m_Position.x = x;
}

float PipePair::GetX() { return m_BottomPipe->m_Position.x; }

int PipePair::RandomY(int min, int max)
{
	return (rand() % (max - min)) + min;
}

Pipe* PipePair::GetBottomPipe() { return m_BottomPipe; }
Pipe* PipePair::GetTopPipe() { return m_TopPipe; }