#include "ZombieAnimator.h"

void ZombieAnimator::Start()
{
    animatedSprite = GetGameObject()->GetComponent<AnimatedSprite>();
}

void ZombieAnimator::Tick(float deltaTime, Vector2 mousePos)
{
    ProcessAnimation();
}

void ZombieAnimator::ProcessAnimation() const
{
    std::string animationToPlay;
    if (isMoving) animationToPlay = currentMoveAnimation;
    else animationToPlay = currentIdleAnimation;

    if (animatedSprite->GetCurrentAnimationState() != animationToPlay)
    {
        animatedSprite->SetAnimationState(animationToPlay);
    }
}

void ZombieAnimator::PlayAttackAnimation() const
{
    animatedSprite->PlayAnimationState(currentAttackAnimation);
}