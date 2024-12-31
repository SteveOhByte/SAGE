#pragma once
#include "../../Engine/SageMath.h"
#include <SpriteBatch.h>

class GameObject;

enum class ComponentType
{
	TRANSFORM,
	PHYSICS,
	COLLIDER,
	SPRITE,
	ANIMATED_SPRITE,
	BUTTON,
	TEXT,
	AUDIO,
	BEHAVIOUR
};

class Component
{
public:
	
	explicit Component(const ComponentType& type)
		: type(type), continuesWhenPaused(false), gameObject(nullptr) {}

	virtual ~Component() = default;

	GameObject* GetGameObject() const { return gameObject; }
	bool ContinuesWhenPaused() const { return continuesWhenPaused; }
	
	ComponentType type;
	bool continuesWhenPaused = false;
	GameObject* gameObject;
};