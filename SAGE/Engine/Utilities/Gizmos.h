#pragma once

#include <DirectXColors.h>
#include <DirectXMath.h>
#include <string>

#include "../../Engine/SageMath.h"

class Sage;

class Gizmos
{
public:
	static void WriteLine(const std::wstring& text, const DirectX::XMVECTORF32& colour = DirectX::Colors::LimeGreen);
	static void WriteLine(const std::string& text, const DirectX::XMVECTORF32& colour = DirectX::Colors::LimeGreen);
	static void DrawLine(const DirectX::SimpleMath::Vector2& start, const DirectX::SimpleMath::Vector2& end, const DirectX::XMVECTORF32& colour = DirectX::Colors::LimeGreen, float duration = 1.0f, float thickness = 1.0f);
	static void DrawBox(const Box& box, const DirectX::XMVECTORF32& colour = DirectX::Colors::LimeGreen, float duration = 1.0f, float thickness = 1.0f);
	static void DrawRing(const Ring& ring, const DirectX::XMVECTORF32& colour = DirectX::Colors::LimeGreen, float duration = 1.0f, float thickness = 1.0f);
	

	static Sage* sage;
};
