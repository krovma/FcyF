#pragma once
#include "Engine/Math/Vec3.hpp"

struct Plane2;
struct Capsule3;
struct Plane3;
struct AABB2;

struct Ray3
{
	Vec3 start = Vec3::ZERO;
	Vec3 dir = Vec3(1,0,0);

	Ray3(const Vec3& start, const Vec3& dir);
	Ray3& operator=(const Ray3& copyFrom) {
		start = copyFrom.start;
		dir = copyFrom.dir;
		return *this;
	}
	static Ray3 FromPoint(const Vec3& start, const Vec3& pointOnRay);

	Vec3 GetPointAt(float k) const;
	Vec3 GetClampedPointAt(float k) const;

	float RaycastToInfCylinder(const Vec3& center, const Vec3& dir, float radius) const;
	float RaycastToSphere(const Vec3& center, float radius) const;
	float RaycastToCapsule3(const Capsule3& capsule) const;
	float RaycastToPlane3(const Plane3& plane) const;
};

struct Ray2
{
	Vec2 start = Vec2::ZERO;
	Vec2 dir = Vec2(1,0);
	Ray2(const Vec2& start, const Vec2& dir);
	Ray2& operator=(const Ray2& copyFrom) {
		start = copyFrom.start;
		dir = copyFrom.dir;
		return *this;
	}
	static Ray2 FromPoint(const Vec2& start, const Vec2& pointOnRay);
	Vec2 GetPointAt(float k) const;
	Vec2 GetClampedPointAt(float k) const;

	float RaycastToPlane2(const Plane2& plane) const;
	float RaycastToAABB2(const AABB2& box) const;
};