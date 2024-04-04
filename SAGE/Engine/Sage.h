#ifndef _ENGINE_H
#define ENGINE_H
#include <unordered_set>
#include <vector>

#include "DirectX.h"
#include "SageMath.h"
#include "Font.h"
#include "Sprite.h"
#include <windows.h>

#include "IColliderListener.h"
#include "IKeyboardListener.h"
#include "IMouseListener.h"
#include "Scene.h"

namespace DirectX {
	class SpriteBatch;
};

class Sage final : public DirectXClass
{
public:
	// Constructors
	explicit Sage(HINSTANCE hInstance);
	~Sage();
	
	void InitializeTextures();
	LRESULT ProcessWindowMessages(UINT msg, WPARAM wParam, LPARAM lParam) override;

	// Called by the render loop
	void Render() override;

	// Called by directX framework
	void Tick(float deltaTime) override;

	// Returns the current scene
	Scene* GetActiveScene()
	{
		for (auto& scene : scenes)
		{
			if (scene->isActive)
			{
				return scene.get();
			}
		}
		return nullptr;
	}

	void ChangeScene(std::string sceneName);
	void Pause();
	void Unpause();
	bool IsPaused() const { return isGamePaused; }

	// Get the centre of the screen
	static int GetCentreX() { return GetScreenWidth() * 0.5f; }
	static int GetCentreY() { return GetScreenHeight() * 0.5f; }

	// Get the screen width and height
	static int GetScreenWidth() { return 1920; }
	static int GetScreenHeight() { return 1080; }

	ID3D11Device* GetDevice() const { return GetD3DDevice(); }

private:
	std::vector<std::unique_ptr<Scene>> scenes;
	std::vector<IKeyboardListener*> keyboardListeners;
	std::unordered_set<UINT> pressedKeys;
	std::vector<IMouseListener*> mouseListeners;
	std::unordered_set<UINT> pressedMouseButtons;
	std::vector<IColliderListener*> colliderListeners;
	
	LPCWSTR sfx;

	bool isGamePaused;

	FontType harmoniaFont;
	SpriteBatch* spriteBatch;

	Vector2 mousePos;
	bool leftButtonDown;
};

#endif