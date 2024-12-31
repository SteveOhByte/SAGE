#ifndef _SPRITE_H
#define _SPRITE_H

#include <iostream>

#include "../../Engine/SageMath.h"
#include <Windows.h>
#include <math.h>

#include "Component.h"
#include "../../Engine/GameObject.h"
#include "../Graphics/TextureType.h"
#include "Transform.h"

class TextureType;
namespace DirectX {
	class SpriteBatch;
}

using float2;
using DirectX::SimpleMath::Color;
using DirectX::SpriteBatch;

class Sprite : public Component
{
public:
	enum Pivot
	{
		UPPER_LEFT,
		UPPER_RIGHT,
		CENTRE,
		LOWER_LEFT,
		LOWER_RIGHT
	};

	Sprite();

	void Initialize(ID3D11Device* device, const wchar_t* fileName, const Color& colour, float layer);
	void Initialize(TextureType* pTexture, const Color& colour, float layer);

	void SetPivot(Pivot pivot);
	void SetTextureRegion(int left, int top, int right, int bottom);
	void Draw(SpriteBatch* pBatch) const;

	int GetWidth() const
	{
		const auto transform = gameObject->GetComponent<Transform>();
		float scale = transform->scale;
		return static_cast<int>(ceilf((textureRegion.right - textureRegion.left) * scale));
	}

	int GetHeight() const
	{
		float scale = gameObject->GetComponent<Transform>()->scale;
		return static_cast<int>(ceilf((textureRegion.bottom - textureRegion.top) * scale));
	}

	float2 GetExtents() const;

	void SetVelocity(float2 velocityPixPerSec, float rotationalVelocityDegPerSec);
	float2 GetVelocity() const { return velocity; }
	float GetRotationalVelocity() const { return rotationalVelocity * 180.0f / 3.1415927f; }
	bool ContainsPoint(float2 pos) const;

	Color GetColour() const { return colour; }
	void SetColour(Color c) { colour = c; }

	void SetTextureAnimation(int frameSizeX, int frameSizeY, int framesPerSecond);
	void UpdateAnimation(float deltaTime);

	bool IsLastFrame() const
	{
		return currentFrame == totalFrames - 1;
	}

	void RestartAnimation() { currentFrame = 0; }

	bool GetVisible() const { return visible; }
	void SetVisible(bool visible) { this->visible = visible; }

private:
	float layer;
	bool visible = true;

	Color colour;
	TextureType* pTexture;

	Pivot pivot;
	float2 origin;

	RECT textureRegion;
	float2 GetCentreNoRotation() const;

	int frameWidth;
	int frameHeight;
	int totalFrames;
	int currentFrame;

	float elapsedTime;
	float frameTime;

	mutable float resolutionScaling = 1.0f;
	
	void SetTextureAnimationRegion();

	float2 velocity;
	float rotationalVelocity;
};

#endif