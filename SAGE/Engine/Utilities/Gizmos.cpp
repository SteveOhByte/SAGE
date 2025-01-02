#include "Gizmos.h"

#include <codecvt>
#include <locale>

#include "../Sage.h"

Sage* Gizmos::sage = nullptr;

void Gizmos::WriteLine(const std::wstring& text, const DirectX::XMVECTORF32& colour)
{
	sage->WriteLine(text, colour);
}

void Gizmos::WriteLine(const std::string& text, const DirectX::XMVECTORF32& colour)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::wstring wideText = converter.from_bytes(text);
	sage->WriteLine(wideText, colour);
}

void Gizmos::DrawLine(const Vector2& start, const Vector2& end, const DirectX::XMVECTORF32& colour, float duration, float thickness)
{
	sage->AddGizmoLine(start, end, colour, duration, thickness);
}

void Gizmos::DrawBox(const Box& box, const DirectX::XMVECTORF32& colour, float duration, float thickness)
{
	sage->AddGizmoLine(box.topLeft, box.topRight, colour, duration, thickness);
	sage->AddGizmoLine(box.topRight, box.bottomRight, colour, duration, thickness);
	sage->AddGizmoLine(box.bottomRight, box.bottomLeft, colour, duration, thickness);
	sage->AddGizmoLine(box.bottomLeft, box.topLeft, colour, duration, thickness);
}

void Gizmos::DrawRing(const Ring& ring, const DirectX::XMVECTORF32& colour, float duration, float thickness)
{
	const float2 centre = ring.centre;
	const float radius = ring.radius;

	const int numSegments = 32;

	std::vector<Vector2> points(numSegments);

	for (int i = 0; i < numSegments; ++i)
	{
		float angle = (static_cast<float>(i) / numSegments) * 360.0f;
		float radians = SageMath::DegToRad(angle);
		points[i].x = centre.x + radius * cosf(radians);
		points[i].y = centre.y + radius * sinf(radians);
	}

	for (int i = 0; i < numSegments; ++i)
	{
		int nextIndex = (i + 1) % numSegments;
		sage->AddGizmoLine(points[i], points[nextIndex], colour, duration, thickness);
	}
}