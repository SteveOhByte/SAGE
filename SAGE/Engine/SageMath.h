#pragma once

#include "d3d11.h"
#include "SimpleMath.h"

#define float2 DirectX::SimpleMath::Vector2

struct Box
{
	float2 topLeft;
	float2 topRight;
	float2 bottomLeft;
	float2 bottomRight;
};

class SageMath
{
public:
	static float Clamp(float value, float min, float max);
	static float LerpAngle(float currentAngle, float targetAngle, float time);
	static float Lerp(float start, float end, float time);
};
