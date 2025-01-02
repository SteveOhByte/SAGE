#include "SageMath.h"

float SageMath::PI = 3.141592653589793f;

float SageMath::Clamp(const float value, const float min, const float max) 
{
	return value < min ? min : (value > max ? max : value);
}

float SageMath::LerpAngle(float startAngle, float endAngle, float t) 
{
	// Ensure t is clamped between 0 and 1
	t = Clamp(t, 0.0f, 1.0f);

	// Handle wraparound for angles:
	float difference = fmodf(endAngle - startAngle, 360.0f);

	// Choose the shortest path (accounting for over 180 degrees)
	if (difference > 180.0f) {
		difference -= 360.0f;
	} else if (difference < -180.0f) {
		difference += 360.0f;
	}

	return startAngle + difference * t;
}

float SageMath::Lerp(float start, float end, float time) {
	return start + (end - start) * Clamp(time, 0.0f, 1.0f);
}

float SageMath::DegToRad(float degrees)
{
	return degrees * PI / 180.0f;
}

float SageMath::RadToDeg(float radians)
{
	return radians * 180.0f / PI;
}

float SageMath::Min(float a, float b)
{
	return a < b ? a : b;
}

float SageMath::Max(float a, float b)
{
	return a > b ? a : b;
}

DirectX::SimpleMath::Vector2 SageMath::NormalizeVector(const DirectX::SimpleMath::Vector2& vector)
{
	const float length = vector.Length();
	if (length == 0.0f)
		return DirectX::SimpleMath::Vector2::Zero;
	else
		return vector / length;
}
