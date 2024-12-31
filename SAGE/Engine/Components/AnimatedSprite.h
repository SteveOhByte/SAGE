#pragma once
#include "Component.h"
#include "Sprite.h"

struct AnimState
{
	std::string name;
	int frameWidth;
	int frameHeight;
	int frameRate;
	TextureType* texture;
};

class AnimatedSprite : public Component
{
public:
	AnimatedSprite() : Component(ComponentType::ANIMATED_SPRITE) {}

	void Initialize(ID3D11Device* device, Color colour, float layer);
	void RegisterAnimationState(const std::string& name, int frameWidth, int frameHeight, int frameRate, const std::wstring& texturePath);
	
	void Draw(SpriteBatch* spriteBatch);
	void Tick(float deltaTime);

	std::string GetCurrentAnimationState();
	void SetAnimationState(const std::string& name);
	void PlayAnimationState(const std::string& name);
	
private:
	std::string currentAnimationState;
	std::string previousAnimationState;
	std::vector<AnimState> animStates;

	bool playingOneShotAnimation = false;
	
	ID3D11Device* device;
	Sprite* sprite;
	Color colour;
	float layer;
};
