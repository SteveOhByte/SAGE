#pragma once
#include "../Engine/Behaviour.h"

class PlayerAnimator : public Behaviour
{
public:
	PlayerAnimator() : Behaviour() {}

	// Behaviour interface
	void Start() override;
	void Tick(float deltaTime, Vector2 mousePos) override;

	void ProcessAnimation() const;

	void SetCurrentMoveAnimation(const std::string& name) { currentMoveAnimation = name; }
	void SetCurrentIdleAnimation(const std::string& name) { currentIdleAnimation = name; }
	void SetCurrentShootAnimation(const std::string& name) { currentShootAnimation = name; }
	void SetCurrentReloadAnimation(const std::string& name) { currentReloadAnimation = name; }
	void SetCurrentMeleeAnimation(const std::string& name) { currentMeleeAnimation = name; }
	
	void SetMoving(bool value) { isMoving = value; }
	void SetMovingUp(bool value) { movingUp = value; }
	void SetMovingDown(bool value) { movingDown = value; }
	void SetMovingLeft(bool value) { movingLeft = value; }
	void SetMovingRight(bool value) { movingRight = value; }
	void PlayShootAnimation() const;
	void PlayReloadAnimation() const;
	void PlayMeleeAnimation() const;

private:
	AnimatedSprite* animatedSprite = nullptr;

	std::string currentMoveAnimation;
	std::string currentIdleAnimation;
	std::string currentShootAnimation;
	std::string currentReloadAnimation;
	std::string currentMeleeAnimation;
	
	bool isMoving = false;
	bool movingUp = false;
	bool movingDown = false;
	bool movingLeft = false;
	bool movingRight = false;
};
