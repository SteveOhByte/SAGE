#pragma once
#include "../Engine/Behaviour.h"
#include "Scenes/GameScene.h"

class GameManager : public Behaviour
{
public:
	GameManager(GameScene* scene) : Behaviour()
	{
		this->scene = scene;
	}

	void Start() override;
	void Tick(float deltaTime, Vector2 mousePos) override;
	
	void OnKeyPress(UINT keyCode) override;

	void Pause() const;
	void Unpause() const;

private:
	GameScene* scene = nullptr;
};
