#pragma once

#include "../../Engine/GameObject.h"
#include "../../Engine/Scene.h"

class MainMenuScene : public Scene
{
public:
	MainMenuScene() : Scene("Main Menu") {}

	void Initialize(ID3D11Device* device) override;
private:
	std::unique_ptr<GameObject> backgroundGameObject;
	std::unique_ptr<GameObject> titleGameObject;
	std::unique_ptr<GameObject> playButtonGameObject;
	std::unique_ptr<GameObject> exitButtonGameObject;
};
