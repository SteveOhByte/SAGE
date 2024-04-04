#pragma once
#include "Component.h"
#include "Sprite.h"
#include "TextureType.h"
#include <functional>

class Button : public Component
{
public:
	using ButtonCallback = std::function<void()>;
	
	Button(ID3D11Device* device, const wchar_t* normalSpriteFileName, const wchar_t* hoverSpriteFileName, const wchar_t* pressedSpriteFileName, const Color& colour, float layer, bool enableHoverEffect = false);

	void Draw(SpriteBatch* pBatch, Vector2 mousePosition, bool leftButtonDown);
	void Tick(float deltaTime);

	void SetHoverEffectParameters(float riseAmount, float transitionSpeed);

	void SetOnClickCallback(ButtonCallback callback);
	void SetOnPressCallback(ButtonCallback callback);
	void SetOnReleaseCallback(ButtonCallback callback);

	ButtonCallback onClickCallback;
	ButtonCallback onPressCallback;
	ButtonCallback onReleaseCallback;
	
	Vector2 mousePosition;
	bool leftButtonDown;
	
	TextureType* normalTex;
	TextureType* hoverTex;
	TextureType* pressedTex;
	
	Sprite* sprite;
	Color colour;
	float layer;

private:
	bool enableHoverEffect;
	float hoverRiseAmount = 10.0f;
	float hoverTransitionSpeed = 5.0f;
	float currentHoverOffset = 0.0f;
	float targetHoverOffset = 0.0f;
	bool wasButtonDown = false;
};
