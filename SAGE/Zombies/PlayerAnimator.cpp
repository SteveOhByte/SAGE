#include "PlayerAnimator.h"

void PlayerAnimator::Start()
{
	animatedSprite = GetGameObject()->GetComponent<AnimatedSprite>();
}

void PlayerAnimator::Tick(float deltaTime, Vector2 mousePos)
{
	ProcessAnimation();
}

void PlayerAnimator::ProcessAnimation() const
{
	std::string animationToPlay;
	if (isMoving) animationToPlay = currentMoveAnimation;
	else animationToPlay = currentIdleAnimation;

	if (animatedSprite->GetCurrentAnimationState() != animationToPlay)
	{
		animatedSprite->SetAnimationState(animationToPlay);
	}
}

void PlayerAnimator::PlayShootAnimation() const
{
	animatedSprite->PlayAnimationState(currentShootAnimation);
}

void PlayerAnimator::PlayReloadAnimation() const
{
	animatedSprite->PlayAnimationState(currentReloadAnimation);
}

void PlayerAnimator::PlayMeleeAnimation() const
{
	animatedSprite->PlayAnimationState(currentMeleeAnimation);
}