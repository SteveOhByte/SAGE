#include "PlayerMovement.h"

void PlayerMovement::Start()
{
	transform = gameObject->GetComponent<Transform>();
	playerAnimator = gameObject->GetComponent<PlayerAnimator>();
}

void PlayerMovement::Tick(float deltaTime, Vector2 mousePos)
{
	isMoving = movingUp || movingDown || movingLeft || movingRight;
	
	ProcessMovement(deltaTime);
	ProcessRotation(mousePos, deltaTime);

	// Pass values to PlayerAnimator
	playerAnimator->SetMoving(isMoving);
	playerAnimator->SetMovingUp(movingUp);
	playerAnimator->SetMovingDown(movingDown);
	playerAnimator->SetMovingLeft(movingLeft);
	playerAnimator->SetMovingRight(movingRight);
}

void PlayerMovement::OnKeyDown(UINT keyCode)
{
	if (keyCode == 'W')
		movingUp = true;
	if (keyCode == 'S')
		movingDown = true;
	if (keyCode == 'A')
		movingLeft = true;
	if (keyCode == 'D')
		movingRight = true;
}

void PlayerMovement::OnKeyUp(UINT keyCode)
{
	if (keyCode == 'W')
		movingUp = false;
	if (keyCode == 'S')
		movingDown = false;
	if (keyCode == 'A')
		movingLeft = false;
	if (keyCode == 'D')
		movingRight = false;
}

void PlayerMovement::ProcessMovement(const float deltaTime) const
{
	Vector2 moveDirection = Vector2(0, 0);
	if (movingUp)
		moveDirection += Vector2(0, -1);
	if (movingDown)
		moveDirection += Vector2(0, 1);
	if (movingLeft)
		moveDirection += Vector2(-1, 0);
	if (movingRight)
		moveDirection += Vector2(1, 0);
    
	if (moveDirection.LengthSquared() > 1.0f) {
		moveDirection.Normalize(); 
	}

	const Vector2 currentPosition = transform->GetPosition();
	const Vector2 targetPosition = currentPosition + moveDirection * moveSpeed * deltaTime;

	const Vector2 newPosition = currentPosition + (targetPosition - currentPosition) * movementSmoothingFactor;

	transform->SetPosition(newPosition);
}

void PlayerMovement::ProcessRotation(Vector2 mousePos, float deltaTime) const
{
	Vector2 playerPos = transform->GetPosition();
	Vector2 direction = mousePos - playerPos;

	float targetAngle = atan2(direction.y, direction.x) * 180.0f / 3.1415927f;

	float currentAngle = transform->GetRotation();
	float newAngle = SageMath::LerpAngle(currentAngle, targetAngle, rotationSpeed * deltaTime);
	transform->SetRotation(newAngle);
}