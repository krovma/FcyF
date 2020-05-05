#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Vec2.hpp"
#include <vector>
#include <string>
//////////////////////////////////////////////////////////////////////////

STATIC const IntVec2 IntVec2::ZERO(0, 0);

STATIC const IntVec2 IntVec2::LOOK_AROUND_4[] = {
	IntVec2(1, 0), //RIGHT
	IntVec2(-1, 0),//LEFT
	IntVec2(0, 1),//UP
	IntVec2(0, -1),//DOWN
};

STATIC const IntVec2 IntVec2::LOOK_AROUND_8[] = {
	IntVec2(1, 0), //RIGHT
	IntVec2(-1, 0),//LEFT
	IntVec2(0, 1),//UP
	IntVec2(0, -1),//DOWN
	IntVec2(1, 1), // RIGHTUP
	IntVec2(-1, -1),//LEFTDOWN
	IntVec2(1, -1),//LEFTUP
	IntVec2(-1, 1),//RIGHTDOWN
};

////////////////////////////////
IntVec2::IntVec2(const IntVec2& copyFrom)
	:x(copyFrom.x)
	,y(copyFrom.y)
{
}

////////////////////////////////
IntVec2::IntVec2(int x, int y)
	:x(x)
	,y(y)
{
}

////////////////////////////////
IntVec2::IntVec2(const Vec2& convertFrom)
	:x((int)convertFrom.x),
	 y((int)convertFrom.y)
{

}

////////////////////////////////
void IntVec2::SetFromText(const char* text)
{
	std::vector<std::string> splited = Split(text, ',');
	if (splited.size() != 2) {
		ERROR_AND_DIE(Stringf("[ERROR]%s: Cannot convert %s to IntVec2", FUNCTION, text));
		return;
	}
	x = std::atoi(splited[0].c_str());
	y = std::atoi(splited[1].c_str());
}

////////////////////////////////
const IntVec2& IntVec2::operator=(const IntVec2& copyFrom)
{
	x = copyFrom.x;
	y = copyFrom.y;
	return *this;
}

////////////////////////////////
const IntVec2& IntVec2::operator-=(const IntVec2& vecToSubtract)
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	return *this;
}

////////////////////////////////
const IntVec2& IntVec2::operator*=(int scale)
{
	x *= scale;
	y *= scale;
	return *this;
}

////////////////////////////////
bool IntVec2::operator!=(const IntVec2& rhs) const
{
	return !(*this == rhs);
}

////////////////////////////////
bool IntVec2::operator==(const IntVec2& rhs) const
{
	return x == rhs.x && y == rhs.y;
}

////////////////////////////////
const IntVec2& IntVec2::operator+=(const IntVec2& vecToAdd)
{
	x += vecToAdd.x;
	y += vecToAdd.y;
	return *this;
}

////////////////////////////////
const IntVec2 IntVec2::operator+(const IntVec2& rhs) const
{
	IntVec2 result(*this);
	result += rhs;
	return result;
}

////////////////////////////////
const IntVec2 IntVec2::operator-(const IntVec2& rhs) const
{
	IntVec2 result(*this);
	result -= rhs;
	return result;
}


////////////////////////////////
const IntVec2 IntVec2::operator*(int rhs) const
{
	IntVec2 result(*this);
	result *= rhs;
	return result;
}

//////////////////////////////////////////////////////////////////////////
const IntVec2 operator* (int lhs, const IntVec2& rhs)
{
	IntVec2 result(rhs);
	result *= lhs;
	return result;
}