#pragma once
#include "Engine/Math/Vec3.hpp"
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------------------------
struct AABB3
{
public:
	static const AABB3 UNIT;

	AABB3();
	AABB3(const AABB3& copyFrom);
	explicit AABB3(const Vec3& min, const Vec3& max);
	explicit AABB3(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);

	/// Format: Vec3;Vec3
	void SetFromText(const char* text);

	float GetWidth() const { return Max.x - Min.x; }
	float GetHeight() const { return Max.y - Min.y; }
	float GetLength() const { return Max.z - Min.z; }
	Vec3 GetCenter() const { return (Min + Max) * 0.5f; }
	/// The extend is already half size from the center
	Vec3 GetExtend() const { return (Max - Min) * 0.5f; }

	bool IsPointInside(const Vec3& point) const;
	bool IsPointInside(float x, float y, float z) const;

	const AABB3 operator+ (const Vec3& translate) const;

public:
	Vec3 Min;
	Vec3 Max;
};


