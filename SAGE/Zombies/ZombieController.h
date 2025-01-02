#pragma once
#include "ZombieAnimator.h"
#include "../Engine/Components/Behaviour.h"

class ZombieController : public Behaviour
{
public:
    ZombieController() : Behaviour() {}

    // Behaviour interface
    void Start() override;
    void Tick(float deltaTime, Vector2 mousePos) override;
    void OnTriggerEnter(GameObject* otherGameObject) override;

    void ProcessMovement(float deltaTime) const;
    void ProcessRotation(Vector2 mousePos, float deltaTime) const;

private:
    Transform* transform = nullptr;
    ZombieAnimator* zombieAnimator = nullptr;

    float movementSmoothingFactor = 1.0f;
    float moveSpeed = 100;
    float rotationSpeed = 10;

    float hp = 3.0f;
	
    bool isMoving = false;
    bool movingUp = false;
    bool movingDown = false;
    bool movingLeft = false;
    bool movingRight = false;
};
