#pragma once

#include "d3d11.h"
#include "SimpleMath.h"

class SageMath
{
public:
	static float Clamp(float value, float min, float max);
	static float LerpAngle(float currentAngle, float targetAngle, float time);
	static float Lerp(float start, float end, float time);
};
