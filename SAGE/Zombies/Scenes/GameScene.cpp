#include "GameScene.h"

#include <DirectXColors.h>

#include "../GameManager.h"
#include "../PlayerInventory.h"
#include "../PlayerMovement.h"
#include "../../Engine/AnimatedSprite.h"
#include "../../Engine/Button.h"
#include "../../Engine/Collider.h"
#include "../../Engine/Sage.h"

void GameScene::Initialize(ID3D11Device* device)
{
	this->device = device;
	
	backgroundGameObject = std::make_unique<GameObject>("Background");
	const auto backgroundTransform = new Transform(Vector2(Sage::GetCentreX(), Sage::GetCentreY()), 0, 1);
	const auto backgroundSprite = new Sprite();
	backgroundSprite->Initialize(device, L"Game\\background.png", DirectX::Colors::White.v, 1);
	backgroundGameObject->AddComponent(backgroundTransform);
	backgroundGameObject->AddComponent(backgroundSprite);

	gameManagerGameObject = std::make_unique<GameObject>("Game Manager");
	const auto gameManager = new GameManager(this);
	gameManagerGameObject->AddComponent(gameManager);
	
	playerGameObject = std::make_unique<GameObject>("Player");
	const auto playerTransform = new Transform(Vector2(Sage::GetCentreX(), Sage::GetCentreY()), 0, 0.25f);
	const auto playerSprite = new AnimatedSprite();
	playerSprite->Initialize(device, DirectX::Colors::White.v, 0);
	playerSprite->RegisterAnimationState("KnifeIdle", 289, 224, 15, L"Game\\Player\\Knife\\idle.png");
	playerSprite->RegisterAnimationState("KnifeAttack", 329, 393, 15, L"Game\\Player\\Knife\\meleeAttack.png");
	playerSprite->RegisterAnimationState("KnifeMove", 279, 219, 15, L"Game\\Player\\Knife\\move.png");
	playerSprite->RegisterAnimationState("PistolIdle", 253, 216, 15, L"Game\\Player\\Pistol\\idle.png");
	playerSprite->RegisterAnimationState("PistolMelee", 291, 256, 15, L"Game\\Player\\Pistol\\meleeAttack.png");
	playerSprite->RegisterAnimationState("PistolMove", 258, 220, 15, L"Game\\Player\\Pistol\\move.png");
	playerSprite->RegisterAnimationState("PistolReload", 260, 230, 15, L"Game\\Player\\Pistol\\reload.png");
	playerSprite->RegisterAnimationState("PistolShoot", 255, 215, 15, L"Game\\Player\\Pistol\\shoot.png");
	playerSprite->RegisterAnimationState("RifleIdle", 313, 207, 15, L"Game\\Player\\Rifle\\idle.png");
	playerSprite->RegisterAnimationState("RifleMelee", 358, 353, 15, L"Game\\Player\\Rifle\\meleeAttack.png");
	playerSprite->RegisterAnimationState("RifleMove", 313, 206, 15, L"Game\\Player\\Rifle\\move.png");
	playerSprite->RegisterAnimationState("RifleReload", 322, 217, 15, L"Game\\Player\\Rifle\\reload.png");
	playerSprite->RegisterAnimationState("RifleShoot", 312, 206, 15, L"Game\\Player\\Rifle\\shoot.png");
	playerSprite->SetAnimationState("KnifeIdle");
	const auto playerMovement = new PlayerMovement();
	const auto playerInventory = new PlayerInventory(this);
	const auto playerAnimator = new PlayerAnimator();
	playerGameObject->AddComponent(playerTransform);
	playerGameObject->AddComponent(playerSprite);
	playerGameObject->AddComponent(playerMovement);
	playerGameObject->AddComponent(playerInventory);
	playerGameObject->AddComponent(playerAnimator);

	zombieGameObject = std::make_unique<GameObject>("Zombie");
	const auto zombieTransform = new Transform(Vector2(Sage::GetCentreX() + 400, Sage::GetCentreY()), 0, 0.25f);
	const auto zombieSprite = new AnimatedSprite();
	zombieSprite->Initialize(device, DirectX::Colors::White.v, 1);
	zombieSprite->RegisterAnimationState("Idle", 243, 224, 15, L"Game\\Zombie\\zombieIdle.png");
	zombieSprite->RegisterAnimationState("Move", 290, 313, 15, L"Game\\Zombie\\zombieMove.png");
	zombieSprite->RegisterAnimationState("Attack", 318, 294, 15, L"Game\\Zombie\\zombieAttack.png");
	zombieSprite->SetAnimationState("Move");
	zombieGameObject->AddComponent(zombieTransform);
	zombieGameObject->AddComponent(zombieSprite);

	pauseTitleGameObject = std::make_unique<GameObject>("Pause Title");
	const auto pauseTitleTransform = new Transform(Vector2(Sage::GetCentreX(), 150), 0, 1);
	const auto pauseTitleSprite = new Sprite();
	pauseTitleSprite->Initialize(device, L"UI\\pauseTitle.png", DirectX::Colors::White.v, 0);
	pauseTitleGameObject->AddComponent(pauseTitleTransform);
	pauseTitleGameObject->AddComponent(pauseTitleSprite);
	pauseTitleGameObject->SetActive(false);

	resumeButtonGameObject = std::make_unique<GameObject>("Resume Button");
	const auto resumeButtonTransform = new Transform(Vector2(Sage::GetCentreX(), Sage::GetCentreY()), 0, 1);
	const auto resumeButtonButton = new Button(device, L"UI\\resumeButtonNormal.png", L"UI\\resumeButtonHover.png", L"UI\\resumeButtonPressed.png", DirectX::Colors::White.v, 0, true);
	resumeButtonButton->SetOnClickCallback([this]
	{
		SetPaused(false);
	});
	resumeButtonGameObject->AddComponent(resumeButtonTransform);
	resumeButtonGameObject->AddComponent(resumeButtonButton);
	resumeButtonGameObject->SetActive(false);

	quitButtonGameObject = std::make_unique<GameObject>("Quit Button");
	const auto quitButtonTransform = new Transform(Vector2(Sage::GetCentreX(), Sage::GetCentreY() + 180), 0, 1);
	const auto quitButtonButton = new Button(device, L"UI\\quitButtonNormal.png", L"UI\\quitButtonHover.png", L"UI\\quitButtonPressed.png", DirectX::Colors::White.v, 0, true);
	quitButtonButton->SetOnClickCallback([]
	{
		PostQuitMessage(0);
	});
	quitButtonGameObject->AddComponent(quitButtonTransform);
	quitButtonGameObject->AddComponent(quitButtonButton);
	quitButtonGameObject->SetActive(false);

	bottomLeftBuildingsGameObject = std::make_unique<GameObject>("Bottom Left Buildings");
	const auto bottomLeftBuildingsTransform = new Transform(Vector2(320, 867), 0, 1);
	const auto bottomLeftBuildingsCollider = new Collider(BOX);
	bottomLeftBuildingsCollider->SetBox(Box2D(bottomLeftBuildingsTransform->position, Vector2(653, 428)));
	bottomLeftBuildingsCollider->DrawCollider(true);
	bottomLeftBuildingsGameObject->AddComponent(bottomLeftBuildingsTransform);
	bottomLeftBuildingsGameObject->AddComponent(bottomLeftBuildingsCollider);
	
	gameObjects.push_back(backgroundGameObject.get());
	gameObjects.push_back(bottomLeftBuildingsGameObject.get());
	gameObjects.push_back(gameManagerGameObject.get());
	gameObjects.push_back(playerGameObject.get());
	gameObjects.push_back(zombieGameObject.get());
	gameObjects.push_back(pauseTitleGameObject.get());
	gameObjects.push_back(resumeButtonGameObject.get());
	gameObjects.push_back(quitButtonGameObject.get());

	isActive = false;
}

bool GameScene::IsPaused()
{
	return sage->IsPaused();
}

void GameScene::SetPaused(bool paused)
{
	if (paused)
		sage->Pause();
	else
		sage->Unpause();
	pauseTitleGameObject->SetActive(paused);
	resumeButtonGameObject->SetActive(paused);
	quitButtonGameObject->SetActive(paused);
}
