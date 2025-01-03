#ifndef _ENGINE_H
#define ENGINE_H
#include <deque>
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
#include "Components/Collider.h"

struct CollisionPair {
	GameObject* objectA;
	GameObject* objectB;
	CollisionResult result;
};

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

	void WriteLine(const std::wstring& text, const DirectX::XMVECTORF32& color);
	void AddGizmoLine(const Vector2& start, const Vector2& end, const DirectX::XMVECTORF32& colour, float duration, float thickness)
	{
		Gizmos::WriteLine("Attempting to draw a gizmo line from " + std::to_string(start.x) + ", " + std::to_string(start.y) + " to " + std::to_string(end.x) + ", " + std::to_string(end.y));
		if (start.y <= 0 || start.x <= 0 || end.y >= Screen::GetHeight() || end.x >= Screen::GetWidth())
		{
			Gizmos::WriteLine("Gizmo line is outside of the screen - discarding");
			return;
		}
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

	struct GizmoLine {
		Vector2 start;
		Vector2 end;
		DirectX::XMVECTORF32 colour;
		float remainingTime;
		float thickness;
	};
	std::vector<GizmoLine> gizmoLines;

	struct GizmoTextLine {
		std::wstring text;
		DirectX::XMVECTORF32 colour;
		float remainingTime;
		static constexpr float FADE_TIME = 5.0f;  // Time in seconds before fading starts
		static constexpr float FADE_DURATION = 1.0f;  // How long the fade out takes
	};
	static const size_t MAX_TEXT_LINES = 7;
	deque<GizmoTextLine> gizmoTextLines;

	std::vector<CollisionPair> DetectCollisions();
	void HandlePhysicsCollision(const CollisionPair& pair, float deltaTime);
	void HandleTriggerCollision(const CollisionPair& pair);

	Vector2 mousePos;
	bool leftButtonDown;
};

#endif