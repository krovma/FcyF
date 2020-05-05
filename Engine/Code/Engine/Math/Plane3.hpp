#pragma once
#include "Engine/Math/Vec3.hpp"

struct Plane3
{
	Plane3() = default;
	Plane3(const Vec3& normal, float distanceToOrigin);
	Plane3(const Vec3& pointOnPlane, const Vec3& normal);

	Vec3 GetPoint() const;
	float GetDistance(const Vec3& position) const;
	inline bool IsFront(const Vec3& position) const { return GetDistance(position) > 0.f; }
	inline bool IsBehind(const Vec3& position) const { return GetDistance(position) < 0.f; }

public:
	Vec3 Normal = Vec3(0.f, 1.f, 0.f);
	float Distance = 0.f;
};