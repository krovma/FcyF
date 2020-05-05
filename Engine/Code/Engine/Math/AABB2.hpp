#pragma once
#include "Engine/Math/Vec2.hpp"
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------------------------
struct AABB2
{
public:
	static const AABB2 UNIT;
	static AABB2 MakeAABB2OfSize(float width, float height);

	AABB2();
	AABB2(const AABB2& copyFrom);
	explicit AABB2(const Vec2& min, const Vec2& max);
	explicit AABB2(float minX, float minY, float maxX, float maxY);

	/// Format: Vec2;Vec2
	void SetFromText(const char* text);
	
	float GetWidth() const { return Max.x - Min.x; }
	float GetHeight() const { return Max.y - Min.y; }
	Vec2 GetCenter() const { return (Min + Max) * 0.5f; }
	/// The extend is already half size from the center
	Vec2 GetExtend() const { return (Max - Min) * 0.5f; }

	bool IsPointInside(const Vec2& point) const;
	bool IsPointInside(float x, float y) const;

	void GrowToIncludePoint(const Vec2& point);
	void GrowToIncludePoint(float x, float y);

	const AABB2 operator+ (const Vec2& translate) const;

	Vec2 GetTopLeft() const { return Vec2(Min.x, Max.y); }
	Vec2 GetTopRight() const { return Max; }
	Vec2 GetBottomLeft() const { return Min; }
	Vec2 GetBottomRight() const { return Vec2(Max.x, Min.y); }

public:
	Vec2 Min;
	Vec2 Max;
};


