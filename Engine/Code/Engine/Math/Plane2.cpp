#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Plane2.hpp"

////////////////////////////////
Plane2::Plane2(const Vec2& normal, float distanceToOrigin)
	: Normal(normal.GetNormalized()), Distance(distanceToOrigin)
{
}

////////////////////////////////
Plane2::Plane2(const Vec2& pointOnPlane, const Vec2& normal)
	:Normal(normal.GetNormalized())
{
	Distance = -pointOnPlane.DotProduct(normal);
}

////////////////////////////////
float Plane2::GetDistance(const Vec2& position) const
{
	return position.DotProduct(Normal) + Distance;
}
