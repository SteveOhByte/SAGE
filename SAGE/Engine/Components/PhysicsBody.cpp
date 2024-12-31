#include "PhysicsBody.h"

void PhysicsBody::Tick(float deltaTime)
{
	velocity.y += gravity * mass * deltaTime;
	velocity *= (1 - drag * deltaTime);
}