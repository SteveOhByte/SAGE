#ifndef _ENGINE_H
#define ENGINE_H
#include <unordered_set>
#include <vector>

#include "Graphics/DirectX.h"
#include "SageMath.h"
#include "Graphics/Font.h"
#include "Components/Sprite.h"
#include <windows.h>

#include "Utilities/IColliderListener.h"
#include "Utilities/IKeyboardListener.h"
#include "Utilities/IMouseListener.h"
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

	ID3D11Device* GetDevice() const { return GetD3DDevice(); }

	void SetGizmoText(const std::wstring& text) { gizmoText = text; }
	void SetGizmoColour(const DirectX::XMVECTORF32& colour) { gizmoColour = colour; }
	void AddGizmoLine(const Vector2& start, const Vector2& end, const DirectX::XMVECTORF32& colour, float duration, float thickness)
	{
		gizmoLines.push_back({start, end, colour, duration, thickness});
	}

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

	wstring gizmoText;
	DirectX::XMVECTORF32 gizmoColour;

	struct GizmoLine {
		Vector2 start;
		Vector2 end;
		DirectX::XMVECTORF32 colour;
		float remainingTime;
		float thickness;
	};
	std::vector<GizmoLine> gizmoLines;

	Vector2 mousePos;
	bool leftButtonDown;
};

#endif