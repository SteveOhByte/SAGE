#include "Collider.h"
#include <iostream>

bool Collider::CheckCollision(const Collider& other, const float2& newPosition)
{
	// Error check
	if (this->type != RECTANGLE && this->type != CIRCLE)
		throw std::invalid_argument("Unsupported collider type for 'this' collider");
	if (other.type != RECTANGLE && other.type != CIRCLE)
		throw std::invalid_argument("Unsupported collider type for 'other' collider");

	// Adjust position based on the type
	const float2 adjustedPosition = this->type == RECTANGLE ? this->square.centre + newPosition : this->circle.centre + newPosition;

	// Check each collision type
	if (this->type == RECTANGLE && other.type == RECTANGLE)
	{
		Rect adjustedBox = this->square;
		adjustedBox.centre = adjustedPosition;
		return RectRectCheck(adjustedBox, other.square).collision;
	}
	else if (this->type == CIRCLE && other.type == CIRCLE)
	{
		Circle adjustedCircle = this->circle;
		adjustedCircle.centre = adjustedPosition;
		return CircleCircleCheck(adjustedCircle, other.circle).collision;
	}
	else if (this->type == RECTANGLE && other.type == CIRCLE)
	{
		Rect adjustedBox = this->square;
		adjustedBox.centre = adjustedPosition;
		return RectCircleCheck(adjustedBox, other.circle).collision;
	}
	else if (this->type == CIRCLE && other.type == RECTANGLE)
	{
		Circle adjustedCircle = this->circle;
		adjustedCircle.centre = adjustedPosition;
		return RectCircleCheck(other.square, adjustedCircle).collision;
	}

	// If this point is reached, the collision check is not supported
	throw std::invalid_argument("Unsupported collider type combination");
}

float2 Collider::Reflect(const Collider& other, float2& velocity, float deltaTime)
{
	float2 normal;
	float2 reflection;

	// Determine the type of 'this' and 'other'
	if (this->type == CIRCLE && other.type == RECTANGLE)
	{
		CollisionResult result = RectCircleCheck(other.square, this->circle);
		if (result.collision)
			normal = result.normal;
	}
	else if (this->type == RECTANGLE && other.type == CIRCLE)
	{
		CollisionResult result = RectCircleCheck(this->square, other.circle);
		if (result.collision)
			normal = result.normal;
	}
	else if (this->type == RECTANGLE && other.type == RECTANGLE)
	{
		CollisionResult result = RectRectCheck(this->square, other.square);
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
	
	if (normal != float2::Zero)
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

CollisionResult Collider::RectRectCheck(const Rect& rectA, const Rect& rectB)
{
	CollisionResult result;
	result.collision = false;
	result.penetrationDepth = 0.0f;
    
	float2 delta = rectA.centre - rectB.centre;
	delta.x = fabsf(delta.x);
	delta.y = fabsf(delta.y);

	const float overlapX = rectA.extents.x + rectB.extents.x - delta.x;
	const float overlapY = rectA.extents.y + rectB.extents.y - delta.y;
    
	// Check if there is an overlap on both axes
	if (overlapX > 0 && overlapY > 0) {
		result.collision = true;

		result.penetrationDepth = SageMath::Min(overlapX, overlapY);
        
		// Determine the axis of least penetration
		if (overlapX < overlapY) {
			// Point of collision is along the x-axis
			const float2 normal = rectA.centre.x > rectB.centre.x ? float2(1, 0) : float2(-1, 0);
			result.normal = normal;
			// Collision point can be approximated on the edge in the direction of the normal
			result.collisionPoint = rectA.centre + normal * rectA.extents.x;
		} else {
			// Point of collision is along the y-axis
			const float2 normal = rectA.centre.y > rectB.centre.y ? float2(0, 1) : float2(0, -1);
			result.normal = normal;
			// Collision point can be approximated on the edge in the direction of the normal
			result.collisionPoint = rectA.centre + normal * rectA.extents.y;
		}
	}
    
	return result;
}

CollisionResult Collider::RectCircleCheck(const Rect& rect, const Circle& circle)
{
	CollisionResult result;
	result.collision = false;
	result.penetrationDepth = 0.0f;

	const float2 centreDistance = circle.centre - rect.centre;
	const float2 clampedDistance = Clamp(centreDistance, -rect.extents, rect.extents);
	const float2 closestPoint = rect.centre + clampedDistance;
	float2 vectorToCircle = circle.centre - closestPoint;
	const float distanceSquared = vectorToCircle.LengthSquared();
	const float radiusSquared = circle.radius * circle.radius;

	if (distanceSquared < radiusSquared)
	{
		result.collision = true;
		result.collisionPoint = closestPoint;
        
		const float distance = sqrtf(distanceSquared);
		// Penetration depth is how far the circle overlaps into the rectangle
		result.penetrationDepth = circle.radius - distance;
        
		// Calculate the normal at the collision point
		result.normal = distance > 0.0f ? vectorToCircle / distance : float2(1, 0);
	}

	return result;
}

CollisionResult Collider::CircleCircleCheck(const Circle& circleA, const Circle& circleB)
{
	CollisionResult result;
	result.collision = false;
	result.penetrationDepth = 0.0f;

	const float2 centreDelta = circleB.centre - circleA.centre;
	const float distance = centreDelta.Length();
	const float radiusSum = circleA.radius + circleB.radius;
    
	if (distance <= radiusSum) {
		result.collision = true;
		result.penetrationDepth = radiusSum - distance;
		
		// Normal is the normalized vector from circleA to circleB
		const float2 normal = centreDelta / distance;
		result.normal = normal;
        
		// Collision point is along the line connecting the centres, scaled by the radius ratio
		const float ratio = circleA.radius / radiusSum;
		result.collisionPoint = circleA.centre + normal * (distance * ratio);
	}
    
	return result;
}

float2 Collider::Clamp(const float2 value, const float2 min, const float2 max)
{
	float2 clamped;
	clamped.x = value.x < min.x ? min.x : value.x > max.x ? max.x : value.x;
	clamped.y = value.y < min.y ? min.y : value.y > max.y ? max.y : value.y;
	return clamped;
}

void Collider::UpdatePosition(const float2& newPosition)
{
	// Update the position based on the collider type
	switch (type)
	{
		case RECTANGLE:
			square.centre = newPosition;
			break;
		case CIRCLE:
			circle.centre = newPosition;
			break;
	}
}

void Collider::Draw()
{
	if (!drawCollider) return;

	if (type == RECTANGLE)
	{
		Box squareGizmo;
		squareGizmo.topLeft = Vector2(square.centre.x - square.extents.x, square.centre.y - square.extents.y);
		squareGizmo.topRight = Vector2(square.centre.x + square.extents.x, square.centre.y - square.extents.y);
		squareGizmo.bottomLeft = Vector2(square.centre.x - square.extents.x, square.centre.y + square.extents.y);
		squareGizmo.bottomRight = Vector2(square.centre.x + square.extents.x, square.centre.y + square.extents.y);
		Gizmos::DrawBox(squareGizmo, DirectX::Colors::LimeGreen, 0.0f, 1.0f);
	}
	else if (type == CIRCLE)
	{
		Ring ringGizmo;
		ringGizmo.centre = circle.centre;
		ringGizmo.radius = circle.radius;
		Gizmos::DrawRing(ringGizmo, DirectX::Colors::LimeGreen, 0.0f, 1.0f);
	}
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
	float2 intersection;
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
		result.normal = float2(0, 0); // TODO: Or calculate a meaningful normal if applicable
	}

	return result;
}