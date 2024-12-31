#include "Gizmos.h"

#include "../Sage.h"

Sage* Gizmos::sage = nullptr;

void Gizmos::WriteText(const std::wstring& text, const DirectX::XMVECTORF32& colour, float size, float thickness)
{
	sage->SetGizmoText(text);
	sage->SetGizmoColour(colour);
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
