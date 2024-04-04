#include "GameManager.h"

void GameManager::Start()
{
	
}

void GameManager::Tick(float deltaTime, Vector2 mousePos)
{
}

void GameManager::OnKeyPress(UINT keyCode)
{
	if (keyCode == VK_ESCAPE)
	{
		if (scene->IsPaused())
		{
			Unpause();
		}
		else
		{
			Pause();
		}
	}
}

void GameManager::Pause() const
{
	scene->SetPaused(true);
}

void GameManager::Unpause() const
{
	scene->SetPaused(false);
}
