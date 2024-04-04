#pragma once
#include <cmath>

#include "Component.h"
#include "SageMath.h"

class Transform : public Component
{
public:
	Transform()
		: Component(ComponentType::TRANSFORM)
	{
		position = DirectX::SimpleMath::Vector2::Zero;
		rotation = 0.0f;
		scale = 1.0f;
	}

	Transform(const DirectX::SimpleMath::Vector2 position, const float rotationInDegrees, const float scale)
		: Component(ComponentType::TRANSFORM)
	{
		this->position = position;
		this->rotation = rotationInDegrees * 3.1415927f / 180.0f;
		this->scale = scale;
	}

	DirectX::SimpleMath::Vector2 GetPosition() const { return position; }
	void SetPosition(const DirectX::SimpleMath::Vector2 p) { position = p; }

	float GetRotation() const { return rotation * 180.0f / 3.1415927f; }
	void SetRotation(const float rotationInDegrees) { rotation = rotationInDegrees * 3.1415927f / 180.0f; }

	float GetScale() const { return scale; }
	void SetScale(const float f) { scale = f; }

	DirectX::SimpleMath::Vector2 GetForward() const
	{
		return -DirectX::SimpleMath::Vector2(std::cos(rotation), sin(rotation));
	}
	DirectX::SimpleMath::Vector2 GetRight() const
	{
		return DirectX::SimpleMath::Vector2(-std::sin(rotation), std::cos(rotation));
	}

	DirectX::SimpleMath::Vector2 position;
	float rotation;
	float scale;
};