#include "PlayerInventory.h"

#include "../Engine/Collider.h"
#include "../Engine/PhysicsBody.h"

void PlayerInventory::Start()
{
	playerAnimator = GetGameObject()->GetComponent<PlayerAnimator>();
	
	weapons.push_back({-1, -1, 10, false, true, "KnifeMove", "KnifeIdle", "KnifeAttack", "KnifeAttack", "KnifeAttack"});
	weapons.push_back({9, 9, 20, true, false, "PistolMove", "PistolIdle", "PistolMelee", "PistolShoot", "PistolReload"});
	weapons.push_back({30, 30, 30, true, false, "RifleMove", "RifleIdle", "RifleMelee", "RifleShoot", "RifleReload"});

	for (int i = 0; i < maxBullets; i++)
	{
		bullets[i] = std::make_unique<GameObject>(gameScene->gameObjects.size());
		const auto bulletTransform = new Transform(Vector2(0, 0), 0, 0.1f);
		
		const auto bulletSprite = new Sprite();
		bulletSprite->Initialize(gameScene->device, L"Game\\bullet.png", DirectX::Colors::White.v, 1);

		const auto bulletCollider = new Collider(CIRCLE);
		const auto bulletPhysics = new PhysicsBody(Vector2(0, 0), 1, 0.1f, 0.0f);
		
		bullets[i]->AddComponent(bulletTransform);
		bullets[i]->AddComponent(bulletSprite);
		bullets[i]->AddComponent(bulletCollider);
		bullets[i]->AddComponent(bulletPhysics);

		bullets[i]->SetActive(false);
	}
}

void PlayerInventory::Tick(float deltaTime, Vector2 mousePos)
{
	playerAnimator->SetCurrentMoveAnimation(GetCurrentWeapon()->moveAnimation);
	playerAnimator->SetCurrentIdleAnimation(GetCurrentWeapon()->idleAnimation);
	playerAnimator->SetCurrentShootAnimation(GetCurrentWeapon()->shootAnimation);
	playerAnimator->SetCurrentReloadAnimation(GetCurrentWeapon()->reloadAnimation);
	playerAnimator->SetCurrentMeleeAnimation(GetCurrentWeapon()->meleeAnimation);
}

void PlayerInventory::OnKeyPress(UINT keyCode)
{
	if (keyCode == '1')
		SelectWeapon(0);
	else if (keyCode == '2')
		SelectWeapon(1);
	else if (keyCode == '3')
		SelectWeapon(2);
	else if (keyCode == 'R')
		Reload();
}

void PlayerInventory::OnButtonClick(int button)
{
	if (button == 1) // Left mouse button
	{
		Weapon* currentWeapon = GetCurrentWeapon();
		if (currentWeapon->isRanged)
			Shoot();
		else
			Melee();
	}
	else if (button == 2) // Right mouse button
		Melee();
}

void PlayerInventory::Shoot()
{
	Weapon* currentWeapon = GetCurrentWeapon();
	if (currentWeapon->currentAmmo > 0)
	{
		playerAnimator->PlayShootAnimation();
		currentWeapon->currentAmmo--;
		
		// Instantiate bullet
		for (auto& bullet : bullets)
		{
			if (!bullet.get()->IsActive())
			{
				Transform* bulletTransform = gameObject->GetComponent<Transform>();
				Vector2 bulletPos = bulletTransform->GetPosition();
				
				Vector2 forward = bulletTransform->GetForward();
				Vector2 right = bulletTransform->GetRight();

				bulletPos += forward * -40;
				bulletPos += right * 14;
				bullet.get()->GetComponent<Transform>()->SetPosition(bulletPos);

				bullet.get()->GetComponent<PhysicsBody>()->SetVelocity(bulletTransform->GetForward() * -bulletSpeed);
				
				bullet.get()->SetActive(true);
				gameScene->AddGameObject(bullet.get());
				
				break;
			}
		}
	}
	else
	{
		// Play empty sfx
	}
}

void PlayerInventory::Reload()
{
	Weapon* currentWeapon = GetCurrentWeapon();
	if (!currentWeapon->isRanged) return;
	
	if (currentWeapon->currentAmmo < currentWeapon->maxAmmo)
		currentWeapon->currentAmmo = currentWeapon->maxAmmo;
	playerAnimator->PlayReloadAnimation();
}

void PlayerInventory::Melee()
{
	playerAnimator->PlayMeleeAnimation();
}