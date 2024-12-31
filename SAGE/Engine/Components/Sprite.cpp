#include "Sprite.h"

#include <codecvt>
#include <SpriteBatch.h>
#include <DirectXColors.h>
#include <iostream>
#include <locale>

#include <math.h>

#include "../Graphics/Screen.h"
#include "../Utilities/Gizmos.h"
using namespace DirectX::SimpleMath;

constexpr float pi = 3.1415927f;
constexpr float twoPi = pi * 2.0f;
inline float DegToRad(const float deg) { return pi * deg * 0.005; }
inline int Clamp(int v, const int min, const int max)
{
	if (v < min) v = min;
	if (v > max) v = max;
	return v;
}

// Constructor
Sprite::Sprite()
	: Component(ComponentType::SPRITE)
{
	colour = Color(DirectX::Colors::White.v);
	pTexture = nullptr;

	pivot = CENTRE;
	origin = float2(0, 0);

	layer = 0.0f;

	textureRegion.left = 0;
	textureRegion.top = 0;
	textureRegion.bottom = 0;
	textureRegion.right = 0;

	frameWidth = 0;
	frameHeight = 0;
	totalFrames = 0;
	currentFrame = 0;
	elapsedTime = 0;
	frameTime = 0;

	rotationalVelocity = 0;
}

// Initialize the sprite with a texture
void Sprite::Initialize(ID3D11Device* device, const wchar_t* fileName, const Color& colour, const float layer)
{
	TextureType* pTex = new TextureType();
	wchar_t path[256];
	wcscpy_s(path, L"..\\Textures\\");
	wcscat_s(path, fileName);
	pTex->Load(device, path);

	Initialize(pTex, colour, layer);
}

// Initialize the sprite
void Sprite::Initialize(TextureType* pTexture, const Color& colour, const float layer)
{
	this->pTexture = pTexture;
	this->colour = colour;
	this->layer = layer;

	if (this->pTexture)
	{
		textureRegion.left = 0;
		textureRegion.top = 0;
		textureRegion.bottom = pTexture->GetHeight();
		textureRegion.right = pTexture->GetWidth();
	}

	// Updates the pivot based on the texture size
	SetPivot(pivot);
}

// Set the pivot point of the sprite
void Sprite::SetPivot(const Pivot pivot)
{
	this->pivot = pivot;

	if (pTexture != nullptr)
	{
		switch (this->pivot)
		{
			case UPPER_LEFT:
				origin = float2(static_cast<float>(textureRegion.left), static_cast<float>(textureRegion.top));
				break;
			case UPPER_RIGHT:
				origin = float2(static_cast<float>(textureRegion.right), static_cast<float>(textureRegion.top));
				break;
			case CENTRE:
				origin = float2(static_cast<float>(textureRegion.right - textureRegion.left) / 2.0f, static_cast<float>(textureRegion.bottom - textureRegion.top) / 2.0f);
				break;
			case LOWER_RIGHT:
				origin = float2(static_cast<float>(textureRegion.right), static_cast<float>(textureRegion.bottom));
				break;
			case LOWER_LEFT:
				origin = float2(static_cast<float>(textureRegion.left), static_cast<float>(textureRegion.bottom));
				break;
		}
	}
}

// Draws the sprite (pBatch must have called Begin() first)
void Sprite::Draw(SpriteBatch* pBatch) const
{
	if (!pTexture || !visible) return;

	float2 referenceResolution = GetGameObject()->referenceResolution;
	if (referenceResolution.x != -1 || referenceResolution.y != -1) // If a reference resolution is specified
	{
		// If the reference resolution is different to the current screen resolution
		if (Screen::GetWidth() != referenceResolution.x || Screen::GetHeight() != referenceResolution.y)
		{
			// Calculate the scaling factors
			float referenceWidthScaling = static_cast<float>(Screen::GetWidth()) / referenceResolution.x;
			float referenceHeightScaling = static_cast<float>(Screen::GetHeight()) / referenceResolution.y;

			// Calculate the scaling factor
			resolutionScaling = referenceWidthScaling < referenceHeightScaling ? referenceWidthScaling : referenceHeightScaling;
		}
	} // Otherwise, use the default scaling factors

	Transform* transform = GetGameObject()->GetComponent<Transform>();
	float2 position = transform->position;
	float rotation = transform->rotation;
	float scale = transform->scale;
	
	pBatch->Draw(pTexture->GetResourceView(), position, &textureRegion, colour, rotation, origin, scale * resolutionScaling, DirectX::SpriteEffects_None, layer);
}

// Set the texture region to show only a portion of the texture
void Sprite::SetTextureRegion(int left, int top, int right, int bottom)
{
	// Verify that the texture region is within the bounds of the texture
	if (pTexture != nullptr)
	{
		left = Clamp(left, 0, pTexture->GetWidth());
		top = Clamp(top, 0, pTexture->GetHeight());
		right = Clamp(right, 0, pTexture->GetWidth());
		bottom = Clamp(bottom, 0, pTexture->GetHeight());
	}

	textureRegion.left = left;
	textureRegion.right = right;
	textureRegion.top = top;
	textureRegion.bottom = bottom;

	// Reset the pivot
	SetPivot(pivot);
}

// Returns true if the point is within the sprite
bool Sprite::ContainsPoint(float2 pos) const
{
	const float halfWidth = 0.5f * static_cast<float>(GetWidth());
	const float halfHeight = 0.5f * static_cast<float>(GetHeight());

	// Get the pivot point of the sprite
	const float2 centre = GetCentreNoRotation();

	Transform* transform = GetGameObject()->GetComponent<Transform>();
	float2 position = transform->position;
	float rotation = transform->rotation;
	
	if (rotation != 0.0f)
	{
		// Rotate the point in the opposite direction
		const Matrix rotMatrix = Matrix::CreateRotationZ(-rotation);
		Vector3 dir(pos.x - position.x, pos.y - position.y, 0.0f);
		dir = Vector3::TransformNormal(dir, rotMatrix);

		// Update the point to a new position in the space of the un-rotated box
		pos = centre - dir;
	}

	return pos.x >= centre.x - halfWidth &&
	pos.x <= centre.x + halfWidth &&
	pos.y >= centre.y - halfHeight &&
	pos.y <= centre.y + halfHeight;
}

// Returns the centre of the sprite
float2 Sprite::GetCentreNoRotation() const
{
	const float halfWidth = 0.5f * static_cast<float>(GetWidth());
	const float halfHeight = 0.5f * static_cast<float>(GetHeight());

	Transform* transform = GetGameObject()->GetComponent<Transform>();
	float2 position = transform->position;
	
	switch (pivot)
	{
		case UPPER_LEFT:
			return position + float2(-halfWidth, -halfHeight);
		case UPPER_RIGHT:
			return position + float2(-halfWidth, -halfHeight);
		case CENTRE:
			return position;
		case LOWER_RIGHT:
			return position + float2(-halfWidth, -halfHeight);
		case LOWER_LEFT:
			return position + float2(-halfWidth, -halfHeight);
	}
	return position;
}

// Sets up the sprite for animation
void Sprite::SetTextureAnimation(const int frameSizeX, const int frameSizeY, const int framesPerSecond)
{
	if (pTexture == nullptr)
		return;

	frameWidth = frameSizeX;
	frameHeight = frameSizeY;
	const int cols = pTexture->GetWidth() / frameWidth;
	const int rows = pTexture->GetHeight() / frameHeight;
	totalFrames = rows * cols;
	currentFrame = 0;

	elapsedTime = 0;
	frameTime = 1.0f / framesPerSecond;

	SetTextureAnimationRegion();
}

// Updates the animation
void Sprite::UpdateAnimation(const float deltaTime)
{
	if (totalFrames > 0)
	{
		elapsedTime += deltaTime;

		// Calculate the number of frames to advance
		const int advanceFrames = static_cast<int>(elapsedTime / frameTime);
		// Use the float modulus operator to get the remaining time
		elapsedTime = fmodf(elapsedTime, frameTime);

		// Advance the frame
		currentFrame = (currentFrame + advanceFrames) % totalFrames;

		// Recalculate the region
		SetTextureAnimationRegion();
	}
}

// Set the texture region based on the current frame
void Sprite::SetTextureAnimationRegion()
{
	// Get the number of columns
	const int cols = pTexture->GetWidth() / frameWidth;

	// Get the column and the row
	const int c = currentFrame % cols;
	const int r = currentFrame / cols;

	// Set the texture region
	textureRegion.left = c * frameWidth;
	textureRegion.right = (c + 1) * frameWidth;
	textureRegion.top = r * frameHeight;
	textureRegion.bottom = (r + 1) * frameHeight;

	// Texture region changed, must reset the pivot
	SetPivot(pivot);
}


// Extents are half the width and half the height
float2 Sprite::GetExtents() const
{
	Transform* transform = GetGameObject()->GetComponent<Transform>();
	float rotation = transform->rotation;
	float scale = transform->scale;
	
	float2 extents(
			(textureRegion.right - textureRegion.left) * scale * 0.5f,
			(textureRegion.bottom - textureRegion.top) * scale * 0.5f
		);

	// Need to adjust it for rotation
	if (rotation != 0)
	{
		float cosTheta = cosf(rotation);
		float sinTheta = sinf(rotation);

		float2 rotated = float2(
				extents.x * cosTheta - extents.y * sinTheta,
				extents.x * sinTheta + extents.y * cosTheta
			);

		extents.x = fabsf(rotated.x);
		extents.y = fabsf(rotated.y);
	}

	return extents;
}

// Set the velocity of the sprite
void Sprite::SetVelocity(const float2 velocityPixPerSec, const float rotationalVelocityDegPerSec)
{
	velocity = velocityPixPerSec;
	rotationalVelocity = DegToRad(rotationalVelocityDegPerSec);
}