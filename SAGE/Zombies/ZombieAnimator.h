#pragma once
#include "../Engine/Components/Behaviour.h"

class ZombieAnimator : public Behaviour
{
public:
    ZombieAnimator() : Behaviour() {}

    // Behaviour interface
    void Start() override;
    void Tick(float deltaTime, Vector2 mousePos) override;

    void ProcessAnimation() const;

    void SetCurrentMoveAnimation(const std::string& name) { currentMoveAnimation = name; }
    void SetCurrentIdleAnimation(const std::string& name) { currentIdleAnimation = name; }
    void SetCurrentAttackAnimation(const std::string& name) { currentAttackAnimation = name; }
	
    void SetMoving(bool value) { isMoving = value; }
    void SetMovingUp(bool value) { movingUp = value; }
    void SetMovingDown(bool value) { movingDown = value; }
    void SetMovingLeft(bool value) { movingLeft = value; }
    void SetMovingRight(bool value) { movingRight = value; }
    void PlayAttackAnimation() const;

private:
    AnimatedSprite* animatedSprite = nullptr;

    std::string currentMoveAnimation;
    std::string currentIdleAnimation;
    std::string currentAttackAnimation;
	
    bool isMoving = false;
    bool movingUp = false;
    bool movingDown = false;
    bool movingLeft = false;
    bool movingRight = false;
};
