#include "AnimatedSprite.h"

#include "TextureType.h"

void AnimatedSprite::Initialize(ID3D11Device* device, Color colour, float layer)
{
	this->device = device;
	this->colour = colour;
	this->layer = layer;
	sprite = new Sprite();
}

void AnimatedSprite::RegisterAnimationState(const std::string& name, int frameWidth, int frameHeight, int frameRate, const std::wstring& texturePath)
{
	auto animState = std::make_unique<AnimState>();
	animState->name = name;
	animState->frameWidth = frameWidth;
	animState->frameHeight = frameHeight;
	animState->frameRate = frameRate;
	animState->texture = new TextureType();

	wchar_t path[256];
	wcscpy_s(path, L"..\\Textures\\");
	wcscat_s(path, texturePath.c_str());
	animState->texture->Load(device, path);
	animStates.push_back(*animState);
}

void AnimatedSprite::Draw(SpriteBatch* spriteBatch)
{
	if (sprite->gameObject == nullptr)
		sprite->gameObject = gameObject;
	
	sprite->Draw(spriteBatch);
}

void AnimatedSprite::Tick(float deltaTime)
{
	if (sprite->gameObject == nullptr)
		sprite->gameObject = gameObject;

	if (playingOneShotAnimation && sprite->IsLastFrame())
	{
		playingOneShotAnimation = false;
		SetAnimationState(previousAnimationState);
	}
	
	sprite->UpdateAnimation(deltaTime);
}

std::string AnimatedSprite::GetCurrentAnimationState()
{
	return currentAnimationState;
}

void AnimatedSprite::SetAnimationState(const std::string& name)
{
	if (playingOneShotAnimation) return;
	for (const auto& animState : animStates)
	{
		if (animState.name == name)
		{
			sprite->Initialize(animState.texture, colour, layer);
			sprite->SetTextureAnimation(animState.frameWidth, animState.frameHeight, animState.frameRate);
			currentAnimationState = name;
			return;
		}
	}
}

void AnimatedSprite::PlayAnimationState(const std::string& name)
{
	for (const auto& animState : animStates)
	{
		if (animState.name == name)
		{
			previousAnimationState = currentAnimationState;
			playingOneShotAnimation = true;
			sprite->Initialize(animState.texture, colour, layer);
			sprite->SetTextureAnimation(animState.frameWidth, animState.frameHeight, animState.frameRate);
			sprite->RestartAnimation();
			return;
		}
	}
}