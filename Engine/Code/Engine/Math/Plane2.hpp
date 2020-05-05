#pragma once
#include "Engine/Math/Vec2.hpp"
//#include "Engine/Math/Vec3.hpp"

struct Plane2
{
	Plane2() = default;
	Plane2(const Vec2& normal, float distanceToOrigin);
	Plane2(const Vec2& pointOnPlane, const Vec2& normal);

	float GetDistance(const Vec2& position) const;
	inline bool IsFront(const Vec2& position) const { return GetDistance(position) > 0.f; }
	inline bool IsBehind(const Vec2& position) const { return GetDistance(position) < 0.f; }

public:
	Vec2 Normal = Vec2(0.f, 1.f);
	float Distance = 0.f;
};