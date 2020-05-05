#pragma once
#include "Engine/Math/Vec2.hpp"

struct Capsule2
{
	Capsule2() = default;
	Capsule2(const Vec2& center, float radius = 0.f);
	Capsule2(const Vec2& start, const Vec2& end, float radius = 0.f);

	Capsule2 operator+(const Vec2& moveBy) const;
	void Move(const Vec2& moveBy);
	void Rotate(float rotationDegrees);
	void SetPosition(const Vec2& position);
	void SetRotation(float rotationDegrees);

	bool IsContaining(const Vec2& position) const;
	Vec2 GetNearestPoint(const Vec2& position) const;
public:
	Vec2 Start = Vec2::ZERO;
	Vec2 End = Vec2::ZERO;
	float Radius = 0.f;
};