#pragma once
#include <string>
#include <utility>
#include <vector>

#include "GameObject.h"
#include "Components/Sprite.h"
#include "Graphics/Screen.h"

class Sage;

class Scene
{
public:
	// converts a given pixels value to a value adjusted for the reference resolution
	float REF(float value) { return value * resolutionScaling; }

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
		gameObject->referenceResolution = referenceResolution;
		gameObjects.push_back(gameObject);
	}

	void SetReferenceResolution(int width, int height)
	{
		referenceResolution = float2(width, height);
		float referenceWidthScaling = static_cast<float>(Screen::GetWidth()) / referenceResolution.x;
		float referenceHeightScaling = static_cast<float>(Screen::GetHeight()) / referenceResolution.y;

		// Calculate the scaling factor
		resolutionScaling = referenceWidthScaling < referenceHeightScaling ? referenceWidthScaling : referenceHeightScaling;
	}

	Sage* sage;
private:
	float2 referenceResolution;
	float resolutionScaling;
};