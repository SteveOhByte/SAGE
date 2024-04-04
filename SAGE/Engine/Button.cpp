#include "Button.h"

Button::Button(ID3D11Device* device, const wchar_t* normalSpriteFileName, const wchar_t* hoverSpriteFileName, const wchar_t* pressedSpriteFileName, const Color& colour, float layer, bool enableHoverEffect)
	: Component(ComponentType::BUTTON), enableHoverEffect(enableHoverEffect)
{
	this->colour = colour;
	this->layer = layer;

	normalTex = new TextureType();
	wchar_t path[256];
	wcscpy_s(path, L"..\\Textures\\");
	wcscat_s(path, normalSpriteFileName);
	normalTex->Load(device, path);
	
	hoverTex = new TextureType();
	path[256];
	wcscpy_s(path, L"..\\Textures\\");
	wcscat_s(path, hoverSpriteFileName);
	hoverTex->Load(device, path);
	
	pressedTex = new TextureType();
	path[256];
	wcscpy_s(path, L"..\\Textures\\");
	wcscat_s(path, pressedSpriteFileName);
	pressedTex->Load(device, path);

	sprite = new Sprite();
	sprite->gameObject = gameObject;
	sprite->Initialize(normalTex, colour, layer);

	continuesWhenPaused = true;
}

void Button::Draw(SpriteBatch* pBatch, Vector2 mousePosition, bool leftButtonDown)
{
	this->mousePosition = mousePosition;
	this->leftButtonDown = leftButtonDown;
	
	if (sprite->gameObject == nullptr)
		sprite->gameObject = gameObject;

	if (enableHoverEffect)
	{
		Transform* transform = sprite->gameObject->GetComponent<Transform>();
		Vector2 hoverEffectPosition = transform->GetPosition();
		hoverEffectPosition.y -= currentHoverOffset;
		transform->SetPosition(hoverEffectPosition);
	}
	
	if (sprite->ContainsPoint(mousePosition))
	{
		if (leftButtonDown)
		{
			sprite->Initialize(pressedTex, colour, layer);
		}
		else
		{
			sprite->Initialize(hoverTex, colour, layer);
		}
	}
	else
	{
		sprite->Initialize(normalTex, colour, layer);
	}

	sprite->Draw(pBatch);

	if (enableHoverEffect)
	{
		Transform* transform = sprite->gameObject->GetComponent<Transform>();
		Vector2 hoverEffectPosition = transform->GetPosition();
		hoverEffectPosition.y += currentHoverOffset;
		transform->SetPosition(hoverEffectPosition);
	}
}

void Button::Tick(float deltaTime)
{
	if (sprite->gameObject == nullptr)
		sprite->gameObject = gameObject;

	bool isOverButton = sprite->ContainsPoint(mousePosition);

	// On press
	if (isOverButton && leftButtonDown && !wasButtonDown)
	{
		if (onPressCallback != nullptr)
			onPressCallback();
	}

	// On release
	if (isOverButton && !leftButtonDown && wasButtonDown)
	{
		if (onReleaseCallback != nullptr)
			onReleaseCallback();
	}
	
	// On click
	if (isOverButton && !leftButtonDown && wasButtonDown)
	{
		if (onClickCallback != nullptr)
			onClickCallback();
	}

	wasButtonDown = leftButtonDown;

	if (enableHoverEffect)
	{
		targetHoverOffset = sprite->ContainsPoint(mousePosition) ? hoverRiseAmount : 0.0f;

		if (std::abs(currentHoverOffset - targetHoverOffset) > 0.01f)
		{
			currentHoverOffset = SageMath::Lerp(currentHoverOffset, targetHoverOffset, hoverTransitionSpeed * deltaTime);
		}
	}
}

void Button::SetHoverEffectParameters(float riseAmount, float transitionSpeed)
{
	hoverRiseAmount = riseAmount;
	hoverTransitionSpeed = transitionSpeed;
}


void Button::SetOnClickCallback(ButtonCallback callback)
{
	onClickCallback = callback;
}

void Button::SetOnPressCallback(ButtonCallback callback)
{
	onPressCallback = callback;
}

void Button::SetOnReleaseCallback(ButtonCallback callback)
{
	onReleaseCallback = callback;
}
