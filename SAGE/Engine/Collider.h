#ifndef _COLLISION_H
#define COLLISION_H
#include "Component.h"
#include "SageMath.h"
using DirectX::SimpleMath::Vector2;

struct Box2D
{
	Box2D()
	{
		centre = Vector2::Zero;
		extents = Vector2::Zero;
	}

	Box2D(const Vector2 centre, const Vector2 extents)
	{
		this->centre = centre;
		this->extents = extents;
	}

	Vector2 centre;
	Vector2 extents;
};

struct Circle
{
	Circle()
	{
		centre = Vector2::Zero;
		radius = 0.0f;
	}

	Circle(const Vector2 centre, const float radius)
	{
		this->centre = centre;
		this->radius = radius;
	}

	Vector2 centre;
	float radius;
};

struct Line2D
{
	Vector2 start;
	Vector2 end;

	Line2D(const Vector2 start, const Vector2 end)
	{
		this->start = start;
		this->end = end;
	}

	Line2D()
	{
		start = Vector2::Zero;
		end = Vector2::Zero;
	}
};

struct CollisionResult
{
	bool collision;
	Vector2 collisionPoint;
	Vector2 normal;
};

enum ColliderType
{
	BOX,
	CIRCLE,
	LINE
};

class Collider : public Component
{
public:
	Collider(ColliderType type = BOX, bool isTrigger = false)
		: Component(ComponentType::COLLIDER)
	{
		this->type = type;
		this->isTrigger = isTrigger;
		this->drawCollider = false;

		switch (this->type)
		{
			case BOX:
				this->box = Box2D();
				break;
			case CIRCLE:
				this->circle = Circle();
				break;
			case LINE:
				this->line = Line2D();
				break;
		}
	}

	bool CheckCollision(const Collider& other, const Vector2& newPosition);
	Vector2 Reflect(const Collider& other, Vector2& velocity, float deltaTime);
	
	CollisionResult BoxBoxCheck(const Box2D& boxA, const Box2D& boxB);
	CollisionResult CircleCircleCheck(const Circle& circleA, const Circle& circleB);
	CollisionResult BoxCircleCheck(const Box2D& box, const Circle& circle);
	CollisionResult LineLineCheck(const Line2D& a, const Line2D& b);
	
	Vector2 Clamp(Vector2 value, Vector2 min, Vector2 max);

	bool IsTrigger() const { return isTrigger; }
	void SetTrigger(bool isTrigger) { this->isTrigger = isTrigger; }

	ColliderType GetType() const { return type; }
	void SetType(ColliderType type) { this->type = type; }

	Box2D GetBox() const { return box; }
	void SetBox(const Box2D& box) { this->box = box; }

	Circle GetCircle() const { return circle; }
	void SetCircle(const Circle& circle) { this->circle = circle; }

	void DrawCollider(bool drawCollider) { this->drawCollider = drawCollider; }
	void Draw();

private:
	bool drawCollider;
	bool isTrigger;
	ColliderType type;
	Box2D box;
	Circle circle;
	Line2D line;
};

#endif