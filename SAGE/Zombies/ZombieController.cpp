#include "ZombieController.h"

#include "../Engine/Utilities/Gizmos.h"

void ZombieController::Start()
{
	transform = gameObject->GetComponent<Transform>();
	zombieAnimator = gameObject->GetComponent<ZombieAnimator>();
}

void ZombieController::Tick(float deltaTime, Vector2 mousePos)
{
	isMoving = movingUp || movingDown || movingLeft || movingRight;
	
	ProcessMovement(deltaTime);
	//ProcessRotation(mousePos, deltaTime);

	// Pass values to ZombieAnimator
	zombieAnimator->SetMoving(isMoving);
	zombieAnimator->SetMovingUp(movingUp);
	zombieAnimator->SetMovingDown(movingDown);
	zombieAnimator->SetMovingLeft(movingLeft);
	zombieAnimator->SetMovingRight(movingRight);
}

void ZombieController::OnTriggerEnter(GameObject* otherGameObject)
{
	Behaviour::OnTriggerEnter(otherGameObject);
	
	if (otherGameObject->HasTag("Bullet"))
	{
		otherGameObject->SetActive(false);
		hp -= 1;

		Gizmos::WriteLine("Zombie has been hit");

		/*if (hp <= 0)
			gameObject->SetActive(false);*/
	}
}

void ZombieController::ProcessMovement(const float deltaTime) const
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

void ZombieController::ProcessRotation(Vector2 mousePos, float deltaTime) const
{
	Vector2 playerPos = transform->GetPosition();
	Vector2 direction = mousePos - playerPos;

	float targetAngle = atan2(direction.y, direction.x) * 180.0f / 3.1415927f;

	float currentAngle = transform->GetRotation();
	float newAngle = SageMath::LerpAngle(currentAngle, targetAngle, rotationSpeed * deltaTime);
	transform->SetRotation(newAngle);
}