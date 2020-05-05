#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"

//////////////////////////////////////////////////////////////////////////
STATIC const AABB2 AABB2::UNIT(0.f, 0.f, 1.f, 1.f);

////////////////////////////////
STATIC AABB2 AABB2::MakeAABB2OfSize(float width, float height)
{
	float x = width * 0.5f;
	float y = height * 0.5f;
	return AABB2(-x, -y, x, y);
}

////////////////////////////////
AABB2::AABB2()
	: Min()
	, Max()
{
}

////////////////////////////////
AABB2::AABB2(const AABB2& copyFrom)
	: Min(copyFrom.Min)
	, Max(copyFrom.Max)
{
}

////////////////////////////////
AABB2::AABB2(const Vec2& min, const Vec2& max)
	: Min(min)
	, Max(max)
{
}

////////////////////////////////
AABB2::AABB2(float minX, float minY, float maxX, float maxY)
	: Min(minX, minY)
	, Max(maxX, maxY)
{
}

////////////////////////////////
void AABB2::SetFromText(const char* text)
{
	std::vector<std::string> splited = Split(text, ';');
	if (splited.size() != 2) {
		ERROR_AND_DIE(Stringf("[ERROR]%s: Cannot convert %s to AABB2", FUNCTION, text));
		return;
	}
	Min.SetFromText(splited[0].c_str());
	Max.SetFromText(splited[1].c_str());
}

////////////////////////////////
bool AABB2::IsPointInside(const Vec2& point) const
{
	return IsPointInside(point.x, point.y);
}

////////////////////////////////
bool AABB2::IsPointInside(float x, float y) const
{
	bool xCheck = (x >= Min.x) && (x <= Max.x);
	bool yCheck = (y >= Min.y) && (y <= Max.y);
	return xCheck && yCheck;
}

////////////////////////////////
void AABB2::GrowToIncludePoint(const Vec2& point)
{
	GrowToIncludePoint(point.x, point.y);
}

////////////////////////////////
void AABB2::GrowToIncludePoint(float x, float y)
{
/*
1|2|3	2 3 5 : Move TopRight(Max)
-+-+-
4|X|5	4 6 7 : Move BottomLeft(Min)
-+-+-
6|7|8	1 8 : Move Both
*/
	if (IsPointInside(x, y)) {
		return;
	}
	if (x > Min.x && y > Max.y) {
		// case 2 3
		Max.x = (Max.x > x ? Max.x : x);
		Max.y = y;
	} else if (x > Max.x && y > Min.y) {
		// case 3[handled] 5
		Max.x = x;
	} else if (x < Min.x && y < Max.y) {
		// case 4 6
		Min.x = x;
		Min.y = (Min.y < y ? Min.y : y);
	} else if (x < Max.x && y < Min.y) {
		// case 6[handled] 7
		Min.y = y;
	} else if (x < Min.x && y > Max.y) {
		// case 1
		Min.x = x;
		Max.y = y;
	} else {
		// case 8
		Max.x = x;
		Min.y = y;
	}
}

////////////////////////////////
const AABB2 AABB2::operator+(const Vec2& translate) const
{
	return AABB2(Min + translate, Max + translate);
}
