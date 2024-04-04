#include "SageMath.h"

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