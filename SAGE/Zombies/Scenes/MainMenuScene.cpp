#include "MainMenuScene.h"

#include <DirectXColors.h>

#include "../../Engine/Components/Button.h"
#include "../../Engine/Sage.h"
#include "../../Engine/Utilities/Gizmos.h"

void MainMenuScene::Initialize(ID3D11Device* device)
{
	SetReferenceResolution(1920, 1080);
	
	backgroundGameObject = std::make_unique<GameObject>("Background");
	const auto backgroundTransform = new Transform(Vector2(Screen::GetCentreX(), Screen::GetCentreY()), 0, 1);
	const auto backgroundSprite = new Sprite();
	backgroundSprite->Initialize(device, L"Menus\\mainMenuBackground.png", DirectX::Colors::White.v, 1);
	backgroundGameObject->AddComponent(backgroundTransform);
	backgroundGameObject->AddComponent(backgroundSprite);

	titleGameObject = std::make_unique<GameObject>("Title");
	const auto titleTransform = new Transform(Vector2(Screen::GetCentreX(), Screen::GetCentreY() - REF(300)), 0, 1);
	const auto titleSprite = new Sprite();
	titleSprite->Initialize(device, L"Menus\\title.png", DirectX::Colors::White.v, 0);
	titleGameObject->AddComponent(titleTransform);
	titleGameObject->AddComponent(titleSprite);
	
	playButtonGameObject = std::make_unique<GameObject>("Play Button");
	const auto playButtonTransform = new Transform(Vector2(Screen::GetCentreX(), Screen::GetCentreY()), 0, 1);
	const auto playButtonButton = new Button(device, L"Menus\\playButtonNormal.png", L"Menus\\playButtonHover.png", L"Menus\\playButtonPressed.png", DirectX::Colors::White.v, 0, true);
	playButtonButton->SetOnClickCallback([this]
	{
		sage->ChangeScene("Game");
	});
	playButtonGameObject->AddComponent(playButtonTransform);
	playButtonGameObject->AddComponent(playButtonButton);
	
	exitButtonGameObject = std::make_unique<GameObject>("Exit Button");
	const auto exitButtonTransform = new Transform(Vector2(Screen::GetCentreX(), Screen::GetCentreY() + REF(180)), 0, 1);
	const auto exitButtonButton = new Button(device, L"Menus\\exitButtonNormal.png", L"Menus\\exitButtonHover.png", L"Menus\\exitButtonPressed.png", DirectX::Colors::White.v, 0, true);
	exitButtonButton->SetOnClickCallback([]
	{
		PostQuitMessage(0);
	});
	exitButtonGameObject->AddComponent(exitButtonTransform);
	exitButtonGameObject->AddComponent(exitButtonButton);

	AddGameObject(backgroundGameObject.get());
	AddGameObject(titleGameObject.get());
	AddGameObject(playButtonGameObject.get());
	AddGameObject(exitButtonGameObject.get());
	isActive = true;
}