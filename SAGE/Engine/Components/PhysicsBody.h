#pragma once
#include "Component.h"
#include "../../Engine/SageMath.h"

class PhysicsBody : public Component
{
public:
	PhysicsBody()
		: Component(ComponentType::PHYSICS)
	{
		velocity = float2(0, 0);
		mass = 1;
		drag = 0.1f;
		gravity = 9.8f;
	}
	
	PhysicsBody(float2 velocity = float2(0, 0), float mass = 1, float drag = 0.1f, float gravity = 9.8f)
		: Component(ComponentType::PHYSICS)
	{
		this->velocity = velocity;
		this->mass = mass;
		this->drag = drag;
		this->gravity = gravity;
	}

	float2 GetVelocity() const { return velocity; }
	void SetVelocity(float2 velocity) { this->velocity = velocity; }

	float GetMass() const { return mass; }
	void SetMass(float mass) { this->mass = mass; }

	float GetDrag() const { return drag; }
	void SetDrag(float drag) { this->drag = drag; }

	float GetGravity() const { return gravity; }
	void SetGravity(float gravity) { this->gravity = gravity; }

	void Tick(float deltaTime);

private:
	float2 velocity;
	float mass;
	float drag;
	float gravity;
};
