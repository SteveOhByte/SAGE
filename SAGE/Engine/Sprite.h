#ifndef _SPRITE_H
#define _SPRITE_H

#include <iostream>

#include "SageMath.h"
#include <Windows.h>
#include <math.h>

#include "Component.h"
#include "GameObject.h"
#include "Transform.h"

class TextureType;
namespace DirectX {
	class SpriteBatch;
}

using DirectX::SimpleMath::Vector2;
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

	Vector2 GetExtents() const;

	void SetVelocity(Vector2 velocityPixPerSec, float rotationalVelocityDegPerSec);
	Vector2 GetVelocity() const { return velocity; }
	float GetRotationalVelocity() const { return rotationalVelocity * 180.0f / 3.1415927f; }
	bool ContainsPoint(Vector2 pos) const;

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
	Vector2 origin;

	RECT textureRegion;
	Vector2 GetCentreNoRotation() const;

	int frameWidth;
	int frameHeight;
	int totalFrames;
	int currentFrame;

	float elapsedTime;
	float frameTime;

	void SetTextureAnimationRegion();

	Vector2 velocity;
	float rotationalVelocity;
};

#endif