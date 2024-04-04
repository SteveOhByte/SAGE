#pragma comment(lib, "Winmm.lib")

#include "Sage.h"

#include <Windowsx.h>
#include <SpriteBatch.h>
#include "Audio.h"
#include "SageMath.h"
#include <DirectXColors.h>
#include <iostream>
#include <random>
#include <sstream>

#include "AnimatedSprite.h"
#include "Behaviour.h"
#include "Button.h"
#include "Collider.h"
#include "Gizmos.h"
#include "PhysicsBody.h"
#include "../Zombies/Scenes/MainMenuScene.h"
#include "../Zombies/Scenes/GameScene.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

// Entry point for the application
int WINAPI WinMain(const HINSTANCE hInstance, HINSTANCE, PSTR, const int nShowCmd)
{
	Sage application(hInstance);

	if (application.InitWindowsApp(L"SAGE", nShowCmd) == false)
	{
		return 0;
	}

	if (application.InitializeDirect3D())
	{
		application.SetDepthStencil(true);
		application.InitializeTextures();
		application.MessageLoop();
	}

	return 0;
}

// Constructor for the application
Sage::Sage(const HINSTANCE hInstance)
	: DirectXClass(hInstance)
{
	mousePos = Vector2(clientWidth * 0.5f, clientHeight * 0.5f);
	spriteBatch = nullptr;

	clearColour = Color(Colors::DarkGray.v);
	isGamePaused = false;
}

// Destructor for the application
Sage::~Sage()
{
	delete spriteBatch;
}

// Initialize the textures used in the game
// TODO: Function poorly named, cleanup needed
void Sage::InitializeTextures()
{
	Gizmos::Initialize(deviceContext, GetDevice());
	spriteBatch = new SpriteBatch(deviceContext);

	std::unique_ptr<Scene> mainMenuScene = std::make_unique<MainMenuScene>();
	mainMenuScene->Initialize(D3DDevice);

	std::unique_ptr<Scene> gameScene = std::make_unique<GameScene>();
	gameScene->Initialize(D3DDevice);

	scenes.push_back(std::move(mainMenuScene));
	scenes.push_back(std::move(gameScene));

	// Pass sage reference to scenes
	for (auto& scene : scenes)
	{
		scene->sage = this;
	}
	
	// Register listeners
	for (auto& gameObject : GetActiveScene()->gameObjects)
	{
		std::vector<Behaviour*> behaviours = gameObject->GetComponentsOfType<Behaviour>();
		for (auto* behaviour : behaviours)
		{
			keyboardListeners.push_back(behaviour);
			mouseListeners.push_back(behaviour);
			colliderListeners.push_back(behaviour);
			behaviour->Start();
		}
	}
}

// Process window messages
LRESULT Sage::ProcessWindowMessages(const UINT msg, const WPARAM wParam, const LPARAM lParam)
{
	switch (msg)
	{
		case WM_MOUSEMOVE:
			mousePos.x = static_cast<float>(GET_X_LPARAM(lParam));
			mousePos.y = static_cast<float>(GET_Y_LPARAM(lParam));
			return 0;
		case WM_LBUTTONDOWN:
			leftButtonDown = true;
			if (pressedMouseButtons.find(1) == pressedMouseButtons.end()) // Button not already pressed
			{
				pressedMouseButtons.insert(1); // Mark as pressed
				for (auto* listener : mouseListeners)
					listener->OnButtonClick(1); // New call for button press
			}
			for (auto* listener : mouseListeners)
				listener->OnButtonDown(1);
			break;
		case WM_LBUTTONUP:
			leftButtonDown = false;
			pressedMouseButtons.erase(1); // Mark as released
			for (auto* listener : mouseListeners)
				listener->OnButtonUp(1);
			break;
		case WM_RBUTTONDOWN:
			if (pressedMouseButtons.find(2) == pressedMouseButtons.end()) // Button not already pressed
			{
				pressedMouseButtons.insert(2); // Mark as pressed
				for (auto* listener : mouseListeners)
					listener->OnButtonClick(2); // New call for button press
			}
			for (auto* listener : mouseListeners)
				listener->OnButtonDown(2);
			break;
		case WM_RBUTTONUP:
			pressedMouseButtons.erase(2); // Mark as released
			for (auto* listener : mouseListeners)
				listener->OnButtonUp(2);
		case WM_MBUTTONDOWN:
			if (pressedMouseButtons.find(3) == pressedMouseButtons.end()) // Button not already pressed
			{
				pressedMouseButtons.insert(3); // Mark as pressed
				for (auto* listener : mouseListeners)
					listener->OnButtonClick(3); // New call for button press
			}
			for (auto* listener : mouseListeners)
				listener->OnButtonDown(3);
			break;
		case WM_MBUTTONUP:
			pressedMouseButtons.erase(3); // Mark as released
			for (auto* listener : mouseListeners)
				listener->OnButtonUp(3);
			break;
		case WM_KEYDOWN:
			if (pressedKeys.find(wParam) == pressedKeys.end()) // Key not already pressed
			{
				pressedKeys.insert(wParam); // Mark as pressed
				for (auto* listener : keyboardListeners)
					listener->OnKeyPress(wParam); // New call for key press
			}
			for (auto* listener : keyboardListeners)
				listener->OnKeyDown(wParam);
			break;
		case WM_KEYUP:
			pressedKeys.erase(wParam); // Mark as released
			for (auto* listener : keyboardListeners)
				listener->OnKeyUp(wParam);
			break;
	}

	// Pass the message
	return DirectXClass::ProcessWindowMessages(msg, wParam, lParam);
}

// Render the game
void Sage::Render()
{
	spriteBatch->Begin(SpriteSortMode_BackToFront);
	
	// Draw the current scene
	for (const auto gameObject : GetActiveScene()->gameObjects)
	{
		if (!gameObject->IsRendered() || !gameObject->IsActive())
			continue;
		
		// Make sure a transform exists
		if (gameObject->GetComponent<Transform>() == nullptr)
			continue;
		
		const Sprite* sprite = gameObject->GetComponent<Sprite>();
		if (sprite != nullptr)
		{
			sprite->Draw(spriteBatch);
		}

		Button* button = gameObject->GetComponent<Button>();
		if (button != nullptr)
		{
			button->Draw(spriteBatch, mousePos, leftButtonDown);
		}

		AnimatedSprite* animatedSprite = gameObject->GetComponent<AnimatedSprite>();
		if (animatedSprite != nullptr)
		{
			animatedSprite->Draw(spriteBatch);
		}

		Collider* collider = gameObject->GetComponent<Collider>();
		if (collider != nullptr)
		{
			collider->Draw();
		}
	}

	spriteBatch->End();

	// render the base class
	DirectXClass::Render();
}

// Update the game
void Sage::Tick(float deltaTime)
{
	for (const auto gameObject : GetActiveScene()->gameObjects)
	{
		if (!gameObject->IsActive())
			continue;
		
		Sprite* sprite = gameObject->GetComponent<Sprite>();
		if (sprite != nullptr)
		{
			if (sprite->ContinuesWhenPaused() || !isGamePaused)
				sprite->UpdateAnimation(deltaTime);
		}

		Button* button = gameObject->GetComponent<Button>();
		if (button != nullptr)
		{
			if (button->ContinuesWhenPaused() || !isGamePaused)
				button->Tick(deltaTime);
		}

		AnimatedSprite* animatedSprite = gameObject->GetComponent<AnimatedSprite>();
		if (animatedSprite != nullptr)
		{
			if (animatedSprite->ContinuesWhenPaused() || !isGamePaused)
				animatedSprite->Tick(deltaTime);
		}

		std::vector<Behaviour*> behaviours = gameObject->GetComponentsOfType<Behaviour>();
		for (auto* behaviour : behaviours)
		{
			if (behaviour->ContinuesWhenPaused() || !isGamePaused)
				behaviour->Tick(deltaTime, mousePos);
		}

		PhysicsBody* physicsBody = gameObject->GetComponent<PhysicsBody>();
		if (physicsBody != nullptr)
		{
			Collider* collider = gameObject->GetComponent<Collider>();
			if (collider == nullptr)
				throw std::exception("PhysicsBody requires a Collider component");
			
			if (physicsBody->ContinuesWhenPaused() || !isGamePaused)
			{
				physicsBody->Tick(deltaTime);
				
				Transform* transform = gameObject->GetComponent<Transform>();
				Vector2 newPosition = transform->GetPosition() + physicsBody->GetVelocity();

				// Check for collisions
				for (const auto otherGameObject : GetActiveScene()->gameObjects)
				{
					if (otherGameObject == gameObject || !otherGameObject->IsActive())
						continue;
					
					Collider* otherCollider = otherGameObject->GetComponent<Collider>();
					if (otherCollider == nullptr)
						continue;
					
					if (collider->CheckCollision(*otherCollider, newPosition))
					{
						// Collision detected
						physicsBody->SetVelocity(collider->Reflect(*otherCollider, physicsBody->GetVelocity(), deltaTime));
					}
				}

				gameObject->GetComponent<Transform>()->SetPosition(newPosition);
			}
		}

		Collider* collider = gameObject->GetComponent<Collider>();
		if (collider != nullptr)
		{
			if (collider->IsTrigger())
			{
				for (const auto otherGameObject : GetActiveScene()->gameObjects)
				{
					if (otherGameObject == gameObject || !otherGameObject->IsActive())
						continue;
					
					Collider* otherCollider = otherGameObject->GetComponent<Collider>();
					if (otherCollider == nullptr)
						continue;
					
					if (collider->CheckCollision(*otherCollider, Vector2::Zero))
					{
						// Collision detected
						for (auto* listener : behaviours)
							listener->OnTriggerEnter(otherGameObject);
					}
				}
			}
		}
	}
}

void Sage::ChangeScene(std::string sceneName)
{
	keyboardListeners.clear();
	mouseListeners.clear();
		
	for (auto& scene : scenes)
	{
		if (scene->name == sceneName)
			scene->isActive = true;
		else
			scene->isActive = false;
	}

	for (auto& gameObject : GetActiveScene()->gameObjects)
	{
		std::vector<Behaviour*> behaviours = gameObject->GetComponentsOfType<Behaviour>();
		for (auto* behaviour : behaviours)
		{
			keyboardListeners.push_back(behaviour);
			mouseListeners.push_back(behaviour);
			behaviour->Start();
		}
	}
}

void Sage::Pause()
{
	isGamePaused = true;
}

void Sage::Unpause()
{
	isGamePaused = false;
}