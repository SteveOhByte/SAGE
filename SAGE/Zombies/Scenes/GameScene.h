#pragma once
#include "../../Engine/GameObject.h"
#include "../../Engine/Scene.h"

class GameScene : public Scene
{
public:
	GameScene() : Scene("Game") {}

	void Initialize(ID3D11Device* device) override;
	
	bool IsPaused();
	void SetPaused(bool paused);

	ID3D11Device* device;
private:
	std::unique_ptr<GameObject> backgroundGameObject;
	std::unique_ptr<GameObject> bottomLeftBuildingsGameObject;
	std::unique_ptr<GameObject> gameManagerGameObject;
	std::unique_ptr<GameObject> playerGameObject;
	std::unique_ptr<GameObject> zombieGameObject;
	std::unique_ptr<GameObject> topBorderGameObject;

	std::unique_ptr<GameObject> pauseTitleGameObject;
	std::unique_ptr<GameObject> resumeButtonGameObject;
	std::unique_ptr<GameObject> quitButtonGameObject;
};
