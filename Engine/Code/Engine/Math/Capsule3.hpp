#pragma once
#include "Engine/Math/Vec3.hpp"

struct Capsule3
{
	Capsule3() = default;
	Capsule3(const Vec3& center, float radius = 0.f);
	Capsule3(const Vec3& start, const Vec3& end, float radius = 0.f);

	Capsule3 operator+(const Vec3& moveBy) const;
	void Move(const Vec3& moveBy);
	void SetPosition(const Vec3& position);

	bool IsContaining(const Vec3& position) const;
	Vec3 GetNearestPoint(const Vec3& position) const;
public:
	Vec3 Start = Vec3::ZERO;
	Vec3 End = Vec3::ZERO;
	float Radius = 0.f;
};