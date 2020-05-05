#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Plane3.hpp"

////////////////////////////////
Plane3::Plane3(const Vec3& normal, float distanceToOrigin)
	: Normal(normal.GetNormalized()), Distance(distanceToOrigin)
{
}

////////////////////////////////
Plane3::Plane3(const Vec3& pointOnPlane, const Vec3& normal)
	:Normal(normal.GetNormalized())
{
	Distance = -pointOnPlane.DotProduct(normal);
}

Vec3 Plane3::GetPoint() const
{
	return -Distance * Normal;
}

////////////////////////////////
float Plane3::GetDistance(const Vec3& position) const
{
	return position.DotProduct(Normal) + Distance;
}
