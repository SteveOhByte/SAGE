#pragma once
#include "PlayerAnimator.h"
#include "../Engine/Components/Behaviour.h"
#include "../Engine/Components/Transform.h"
#include "../Engine/GameObject.h"

class PlayerMovement : public Behaviour
{
public:
	PlayerMovement() : Behaviour() {}

	// Behaviour interface
	void Start() override;
	void Tick(float deltaTime, Vector2 mousePos) override;
	void OnKeyDown(UINT keyCode) override;
	void OnKeyUp(UINT keyCode) override;

	void ProcessMovement(float deltaTime) const;
	void ProcessRotation(Vector2 mousePos, float deltaTime) const;

private:
	Transform* transform = nullptr;
	PlayerAnimator* playerAnimator = nullptr;

	float movementSmoothingFactor = 1.0f;
	float moveSpeed = 100;
	float rotationSpeed = 10;
	
	bool isMoving = false;
	bool movingUp = false;
	bool movingDown = false;
	bool movingLeft = false;
	bool movingRight = false;
};
