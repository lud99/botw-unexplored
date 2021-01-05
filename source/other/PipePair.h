#pragma once

#include "Pipe.h"

class PipePair
{
private:
	NWindow *m_Window = nullptr;

	Pipe* m_BottomPipe;
	Pipe* m_TopPipe;

public:
	struct Positions
	{
		float bottomY, topY;

		Positions(float _bottomY, float _topY) {
			bottomY = _bottomY;
			topY = _topY;
		}
	};

public:
    PipePair();
	PipePair(NWindow* window);

	void Update();
	void Draw();

	static int RandomY(int max, int min);

	Positions GenerateY();

	void SetX(float x);
	float GetX();

	Pipe* GetTopPipe();
	Pipe* GetBottomPipe();

    ~PipePair();
};