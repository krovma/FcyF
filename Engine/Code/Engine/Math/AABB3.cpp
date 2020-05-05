#include "Engine/Math/AABB3.hpp"
#include "Engine/Core/EngineCommon.hpp"
//////////////////////////////////////////////////////////////////////////
STATIC const AABB3 AABB3::UNIT(Vec3::ZERO, Vec3::ONE);
////////////////////////////////
AABB3::AABB3()
	: Min()
	, Max()
{
}

////////////////////////////////
AABB3::AABB3(const AABB3& copyFrom)
	: Min(copyFrom.Min)
	, Max(copyFrom.Max)
{
}

////////////////////////////////
AABB3::AABB3(const Vec3& min, const Vec3& max)
	: Min(min)
	, Max(max)
{
}

////////////////////////////////
AABB3::AABB3(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
	: Min(minX, minY, minZ)
	, Max(maxX, maxY, maxZ)
{
}

////////////////////////////////
void AABB3::SetFromText(const char* text)
{
	std::vector<std::string> splited = Split(text, ';');
	if (splited.size() != 2) {
		ERROR_AND_DIE(Stringf("[ERROR]%s: Cannot convert %s to AABB3", FUNCTION, text));
		return;
	}
	Min.SetFromText(splited[0].c_str());
	Max.SetFromText(splited[1].c_str());
}

////////////////////////////////
bool AABB3::IsPointInside(const Vec3& point) const
{
	return IsPointInside(point.x, point.y, point.z);
}

////////////////////////////////
bool AABB3::IsPointInside(float x, float y, float z) const
{
	bool xCheck = (x >= Min.x) && (x <= Max.x);
	bool yCheck = (y >= Min.y) && (y <= Max.y);
	bool zCheck = (z >= Min.z) && (z <= Max.z);
	return xCheck && yCheck && zCheck;
}

////////////////////////////////
const AABB3 AABB3::operator+(const Vec3& translate) const
{
	return AABB3(Min + translate, Max + translate);
}
