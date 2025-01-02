#ifndef _COLLISION_H
#define COLLISION_H
#include "Component.h"
#include "../../Engine/Utilities/Gizmos.h"
using float2;

struct Rect
{
	Rect()
	{
		centre = float2::Zero;
		extents = float2::Zero;
	}

	Rect(const float2 centre, const float2 extents)
	{
		this->centre = centre;
		this->extents = extents;
	}

	float2 centre;
	float2 extents;
};

struct Circle
{
	Circle()
	{
		centre = float2::Zero;
		radius = 0.0f;
	}

	Circle(const float2 centre, const float radius)
	{
		this->centre = centre;
		this->radius = radius;
	}

	float2 centre;
	float radius;
};

struct Line2D
{
	float2 start;
	float2 end;

	Line2D(const float2 start, const float2 end)
	{
		this->start = start;
		this->end = end;
	}

	Line2D()
	{
		start = float2::Zero;
		end = float2::Zero;
	}
};

struct CollisionResult
{
	bool collision;
	float2 collisionPoint;
	float2 normal;
	float penetrationDepth;
};

enum ColliderType
{
	RECTANGLE,
	CIRCLE,
	LINE
};

class Collider : public Component
{

public:
	Collider(ColliderType type = RECTANGLE, bool isTrigger = false)
		: Component(ComponentType::COLLIDER)
	{
		this->type = type;
		this->isTrigger = isTrigger;
		this->drawCollider = false;

		switch (this->type)
		{
			case RECTANGLE:
				this->square = Rect();
				break;
			case CIRCLE:
				this->circle = Circle();
				break;
			case LINE:
				this->line = Line2D();
				break;
		}
	}

	bool CheckCollision(const Collider& other, const float2& newPosition);
	float2 Reflect(const Collider& other, float2& velocity, float deltaTime);
	
	CollisionResult RectRectCheck(const Rect& rectA, const Rect& rectB);
	CollisionResult CircleCircleCheck(const Circle& circleA, const Circle& circleB);
	CollisionResult RectCircleCheck(const Rect& rect, const Circle& circle);
	CollisionResult LineLineCheck(const Line2D& a, const Line2D& b);
	
	float2 Clamp(float2 value, float2 min, float2 max);

	void UpdatePosition(const float2& newPosition);
	
	bool IsTrigger() const { return isTrigger; }
	void SetTrigger(bool isTrigger) { this->isTrigger = isTrigger; }

	ColliderType GetType() const { return type; }
	void SetType(ColliderType type) { this->type = type; }

	Rect GetSquare() const { return square; }
	void SetSquare(const Rect& square) { this->square = square; }

	Circle GetCircle() const { return circle; }
	void SetCircle(const Circle& circle) { this->circle = circle; }

	void DrawCollider(bool drawCollider) { this->drawCollider = drawCollider; }
	void Draw();

private:
	bool drawCollider;
	bool isTrigger;
	ColliderType type;
	Rect square;
	Circle circle;
	Line2D line;
};

#endif