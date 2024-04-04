#pragma once
#include "PlayerAnimator.h"
#include "../Engine/Behaviour.h"
#include "Scenes/GameScene.h"

struct Weapon
{
	int maxAmmo;
	int currentAmmo;
	int damage;
	bool isRanged;
	bool isEquipped;
	std::string moveAnimation;
	std::string idleAnimation;
	std::string meleeAnimation;
	std::string shootAnimation;
	std::string reloadAnimation;
};

class PlayerInventory : public Behaviour
{
public:
	PlayerInventory(GameScene* scene) : Behaviour()
	{
		this->gameScene = scene;
	}

	// Behaviour interface
	void Start() override;
	void Tick(float deltaTime, Vector2 mousePos) override;
	void OnKeyPress(UINT keyCode) override;
	void OnButtonClick(int button) override;

	Weapon* GetCurrentWeapon()
	{
		for (int i = 0; i < weapons.size(); i++)
		{
			if (weapons[i].isEquipped)
				return &weapons[i];
		}
		return &weapons[0];
	}
	void SelectWeapon(const int index)
	{
		for (int i = 0; i < weapons.size(); i++)
			weapons[i].isEquipped = false;
		weapons[index].isEquipped = true;
	}

	void Shoot();
	void Reload();
	void Melee();

private:
	static constexpr int maxBullets = 100;
	static constexpr float bulletSpeed = 15.0f;
	PlayerAnimator* playerAnimator = nullptr;
	GameScene* gameScene = nullptr;
	
	std::vector<Weapon> weapons;
	std::unique_ptr<GameObject> bullets[maxBullets] = {};
};
