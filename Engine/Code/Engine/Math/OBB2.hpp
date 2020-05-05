#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/AABB2.hpp"

struct Plane2;
struct Capsule2;

struct OBB2
{
public:
	OBB2();
	OBB2(const Vec2& center, const Vec2& size = Vec2::ZERO, float rotationDegrees = 0.f);
	explicit OBB2(const AABB2& convertFrom);
	explicit OBB2(const Capsule2& convertFrom);

	OBB2 operator+(const Vec2& moveBy) const;
	void Move(const Vec2& moveBy);
	void Rotate(float rotationDegrees);
	void SetPosition(const Vec2& position);
	void SetRotation(float rotationDegrees);

	inline Vec2 GetTopLeft() const		{ return Center - Extends.x * Right + Extends.y * Right.GetRotated90Degrees(); }
	inline Vec2 GetTopRight() const		{ return Center + Extends.x * Right + Extends.y * Right.GetRotated90Degrees(); }
	inline Vec2 GetBottomLeft() const	{ return Center - Extends.x * Right - Extends.y * Right.GetRotated90Degrees(); }
	inline Vec2 GetBottomRight() const	{ return Center + Extends.x * Right - Extends.y * Right.GetRotated90Degrees(); }
	inline Vec2 GetSize() const { return Extends * 2.f; }


	Vec2 WorldToLocal(const Vec2& worldPosition) const;
	Vec2 LocalToWorld(const Vec2& localPosition) const;

	Vec2 GetNearestPoint(const Vec2& worldPoint) const;
	bool IsIntersectWith(const OBB2& with) const;
	bool IsContaining(const Vec2& worldPosition) const;

	AABB2 GetBounding() const;

	void GetPlanes(Plane2* out) const;
	void GetCorners(Vec2* out) const;

public:
	Vec2 Center = Vec2::ZERO;
	Vec2 Right = Vec2(1.f, 0.f);
	Vec2 GetRight() const;
	Vec2 Extends = Vec2::ZERO;
};