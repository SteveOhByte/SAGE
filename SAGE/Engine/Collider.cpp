#include "Collider.h"
#include <iostream>

bool Collider::CheckCollision(const Collider& other, const Vector2& newPosition)
{
	// Error check
	if (this->type != BOX && this->type != CIRCLE)
		throw std::invalid_argument("Unsupported collider type for 'this' collider");
	if (other.type != BOX && other.type != CIRCLE)
		throw std::invalid_argument("Unsupported collider type for 'other' collider");

	// Adjust position based on the type
	const Vector2 adjustedPosition = this->type == BOX ? this->box.centre + newPosition : this->circle.centre + newPosition;

	// Check each collision type
	if (this->type == BOX && other.type == BOX)
	{
		Box2D adjustedBox = this->box;
		adjustedBox.centre = adjustedPosition;
		return BoxBoxCheck(adjustedBox, other.box).collision;
	}
	else if (this->type == CIRCLE && other.type == CIRCLE)
	{
		Circle adjustedCircle = this->circle;
		adjustedCircle.centre = adjustedPosition;
		return CircleCircleCheck(adjustedCircle, other.circle).collision;
	}
	else if (this->type == BOX && other.type == CIRCLE)
	{
		Box2D adjustedBox = this->box;
		adjustedBox.centre = adjustedPosition;
		return BoxCircleCheck(adjustedBox, other.circle).collision;
	}
	else if (this->type == CIRCLE && other.type == BOX)
	{
		Circle adjustedCircle = this->circle;
		adjustedCircle.centre = adjustedPosition;
		return BoxCircleCheck(other.box, adjustedCircle).collision;
	}

	// If this point is reached, the collision check is not supported
	throw std::invalid_argument("Unsupported collider type combination");
}

Vector2 Collider::Reflect(const Collider& other, Vector2& velocity, float deltaTime)
{
	Vector2 normal;
	Vector2 reflection;

	// Determine the type of 'this' and 'other'
	if (this->type == CIRCLE && other.type == BOX)
	{
		CollisionResult result = BoxCircleCheck(other.box, this->circle);
		if (result.collision)
			normal = result.normal;
	}
	else if (this->type == BOX && other.type == CIRCLE)
	{
		CollisionResult result = BoxCircleCheck(this->box, other.circle);
		if (result.collision)
			normal = result.normal;
	}
	else if (this->type == BOX && other.type == BOX)
	{
		CollisionResult result = BoxBoxCheck(this->box, other.box);
		if (result.collision)
			normal = result.normal;
	}
	else if (this->type == CIRCLE && other.type == CIRCLE)
	{
		CollisionResult result = CircleCircleCheck(this->circle, other.circle);
		if (result.collision)
			normal = result.normal;
	}
	else if (this->type == LINE && other.type == LINE)
	{
		CollisionResult result = LineLineCheck(this->line, other.line);
		if (result.collision)
			normal = result.normal;
	}
	else
	{
		throw std::invalid_argument("Unsupported collider type combination");
	}
	
	if (normal != Vector2::Zero)
	{
		float dotProduct = velocity.Dot(normal);
		reflection = velocity - 2 * dotProduct * normal;
	}
	else
	{
		reflection = velocity;
	}

	return reflection;
}

CollisionResult Collider::BoxBoxCheck(const Box2D& boxA, const Box2D& boxB)
{
	CollisionResult result;
	result.collision = false;
    
	Vector2 delta = boxA.centre - boxB.centre;
	delta.x = fabsf(delta.x);
	delta.y = fabsf(delta.y);

	const float overlapX = boxA.extents.x + boxB.extents.x - delta.x;
	const float overlapY = boxA.extents.y + boxB.extents.y - delta.y;
    
	// Check if there is an overlap on both axes
	if (overlapX > 0 && overlapY > 0) {
		result.collision = true;
        
		// Determine the axis of least penetration
		if (overlapX < overlapY) {
			// Point of collision is along the x-axis
			const Vector2 normal = boxA.centre.x > boxB.centre.x ? Vector2(1, 0) : Vector2(-1, 0);
			result.normal = normal;
			// Collision point can be approximated on the edge in the direction of the normal
			result.collisionPoint = boxA.centre + normal * boxA.extents.x;
		} else {
			// Point of collision is along the y-axis
			const Vector2 normal = boxA.centre.y > boxB.centre.y ? Vector2(0, 1) : Vector2(0, -1);
			result.normal = normal;
			// Collision point can be approximated on the edge in the direction of the normal
			result.collisionPoint = boxA.centre + normal * boxA.extents.y;
		}
	}
    
	return result;
}

CollisionResult Collider::BoxCircleCheck(const Box2D& box, const Circle& circle)
{
	CollisionResult result;
	result.collision = false;

	// Calculate the distance from the circle's centre to the box's center
	const Vector2 centreDistance = circle.centre - box.centre;

	const Vector2 clampedDistance = Clamp(centreDistance, -box.extents, box.extents);

	// Closest point on the box to the circle's centre
	const Vector2 closestPoint = box.centre + clampedDistance;

	// Vector from the closest point to the circle's centre
	Vector2 vectorToCircle = circle.centre - closestPoint;

	if (vectorToCircle.LengthSquared() < circle.radius * circle.radius)
	{
		result.collision = true;
		// Collision point is the closest point on the box to the circle
		result.collisionPoint = closestPoint;
		// Calculate the normal at the collision point
		vectorToCircle.Normalize();
		result.normal = vectorToCircle;
	}

	return result;
}

CollisionResult Collider::CircleCircleCheck(const Circle& circleA, const Circle& circleB)
{
	CollisionResult result;
	result.collision = false;

	const Vector2 centreDelta = circleB.centre - circleA.centre;
	const float distance = centreDelta.Length();
	const float radiusSum = circleA.radius + circleB.radius;
    
	if (distance <= radiusSum) {
		result.collision = true;
		// Normal is the normalized vector from circleA to circleB
		const Vector2 normal = centreDelta / distance;
		result.normal = normal;
        
		// Collision point is along the line connecting the centres, scaled by the radius ratio
		const float ratio = circleA.radius / radiusSum;
		result.collisionPoint = circleA.centre + normal * (distance * ratio);
	}
    
	return result;
}

Vector2 Collider::Clamp(const Vector2 value, const Vector2 min, const Vector2 max)
{
	Vector2 clamped;
	clamped.x = value.x < min.x ? min.x : value.x > max.x ? max.x : value.x;
	clamped.y = value.y < min.y ? min.y : value.y > max.y ? max.y : value.y;
	return clamped;
}

void Collider::Draw()
{
	if (!drawCollider) return;

	/*switch (type)
	{
		case BOX:
			DrawBox(box);
			break;
	}*/
}

inline bool IsZeroToOne(const float t) { return t >= 0 && t <= 1; }

CollisionResult Collider::LineLineCheck(const Line2D& a, const Line2D& b)
{
	CollisionResult result;
	result.collision = false;

	// Start and end points
	const float x1 = a.start.x;
	const float y1 = a.start.y;
	const float x2 = a.end.x;
	const float y2 = a.end.y;

	const float x3 = b.start.x;
	const float y3 = b.start.y;
	const float x4 = b.end.x;
	const float y4 = b.end.y;

	// Calculate the denominator
	const float denominator = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

	// Parallel line check
	if (fabsf(denominator) < 0.0001f) {
		return result; // No collision
	}

	// Calculate intersection point
	Vector2 intersection;
	intersection.x = ((x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4)) / denominator;
	intersection.y = ((x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4)) / denominator;

	float tA, tB;
	if (fabsf(x2 - x1) > fabsf(y2 - y1))
		tA = (intersection.x - x1) / (x2 - x1);
	else
		tA = (intersection.y - y1) / (y2 - y1);

	if (fabsf(x4 - x3) > fabsf(y4 - y3))
		tB = (intersection.x - x3) / (x4 - x3);
	else
		tB = (intersection.y - y3) / (y4 - y3);

	// Check if intersection is within the segments
	if (tA >= 0 && tA <= 1 && tB >= 0 && tB <= 1) {
		result.collision = true;
		result.collisionPoint = intersection;
		// Since normal is not directly meaningful for line-line intersection,
		// it's set to zero or could be calculated based on the context if needed.
		result.normal = Vector2(0, 0); // TODO: Or calculate a meaningful normal if applicable
	}

	return result;
}