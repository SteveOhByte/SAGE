#pragma once
#include <string>
#include <utility>
#include <vector>

#include "GameObject.h"
#include "Sprite.h"

class Sage;

class Scene
{
public:
	std::string name;
	std::vector<GameObject*> gameObjects;
	bool isActive;

	explicit Scene(const std::string& name)
	{
		this->name = std::move(name);
		this->gameObjects = std::vector<GameObject*>();
		this->isActive = false;
	}

	virtual void Initialize(ID3D11Device* device) = 0;

	void AddGameObject(GameObject* gameObject)
	{
		gameObject->sage = sage;
		gameObject->GenerateUID(gameObjects);
		gameObjects.push_back(gameObject);
	}

	Sage* sage;
};