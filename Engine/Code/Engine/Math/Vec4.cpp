#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec2.hpp"

////////////////////////////////
Vec4::Vec4(const Vec4& copyFrom)
	: x(copyFrom.x)
	, y(copyFrom.y)
	, z(copyFrom.z)
	, w(copyFrom.w)
{
}

////////////////////////////////
Vec4::Vec4(const Vec3& extendFrom, float w/*=0.f*/)
	: x(extendFrom.x)
	, y(extendFrom.y)
	, z(extendFrom.z)
	, w(w)
{
}

////////////////////////////////
Vec4::Vec4(const Vec2& extendFrom, float z/*=0.f*/, float w/*=0.f*/)
	: x(extendFrom.x)
	, y(extendFrom.y)
	, z(z)
	, w(w)
{
}

////////////////////////////////
Vec4::Vec4(float x, float y, float z, float w)
	: x(x)
	, y(y)
	, z(z)
	, w(w)
{
}

////////////////////////////////
void Vec4::SetFromText(const char* text)
{
	std::vector<std::string> splited = Split(text, ',');
	if (splited.size() != 4) {
		ERROR_AND_DIE(Stringf("[ERROR]%s: Cannot convert %s to Vec4", FUNCTION, text));
		return;
	}
	x = (float)std::atof(splited[0].c_str());
	y = (float)std::atof(splited[1].c_str());
	z = (float)std::atof(splited[2].c_str());
	w = (float)std::atof(splited[3].c_str());
}

////////////////////////////////
void Vec4::operator=(const Vec4& copyFrom)
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
	w = copyFrom.w;
}

////////////////////////////////
const Vec4 Vec4::operator+(const Vec4& vecToAdd) const
{
	Vec4 result;
	result.x = x + vecToAdd.x;
	result.y = y + vecToAdd.y;
	result.z = z + vecToAdd.z;
	result.w = w + vecToAdd.w;
	return result;
}

////////////////////////////////
const Vec4 Vec4::operator-(const Vec4& vecToSubtract) const
{
	Vec4 result;
	result.x = x - vecToSubtract.x;
	result.y = y - vecToSubtract.y;
	result.z = z - vecToSubtract.z;
	result.w = w - vecToSubtract.w;
	return result;
}

////////////////////////////////
const Vec4 Vec4::operator-() const
{
	Vec4 result;
	result.x = -x;
	result.y = -y;
	result.z = -z;
	result.w = -w;
	return result;
}

////////////////////////////////
const Vec4 Vec4::operator*(float uniformScale) const
{
	Vec4 result;
	result.x = x * uniformScale;
	result.y = y * uniformScale;
	result.z = z * uniformScale;
	result.w = w * uniformScale;
	return result;
}

////////////////////////////////
const Vec4 Vec4::operator/(float inverseScale) const
{
	Vec4 result;
	result.x = x / inverseScale;
	result.y = y / inverseScale;
	result.z = z / inverseScale;
	result.w = w / inverseScale;
	return result;
}

////////////////////////////////
void Vec4::operator-=(const Vec4& vecToSubtract)
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	z -= vecToSubtract.z;
	w -= vecToSubtract.w;
}

////////////////////////////////
void Vec4::operator/=(const float inverseScale)
{
	x /= inverseScale;
	y /= inverseScale;
	z /= inverseScale;
	w /= inverseScale;
}

////////////////////////////////
void Vec4::operator*=(const float uniformScale)
{
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
	w *= uniformScale;
}

////////////////////////////////
void Vec4::operator+=(const Vec4& vecToAdd)
{
	x += vecToAdd.x;
	y += vecToAdd.y;
	z += vecToAdd.z;
	w += vecToAdd.w;
}

////////////////////////////////
bool Vec4::operator==(const Vec4& compare) const
{
	return
	(	x == compare.x
	&&	y == compare.y
	&&	z == compare.z
	&&	w == compare.w
	);
}

////////////////////////////////
bool Vec4::operator!=(const Vec4& compare) const
{
	return !(operator==(compare));
}

////////////////////////////////
float Vec4::DotProduct(const Vec4& dotProductWith) const
{
	return (x*dotProductWith.x + y * dotProductWith.y + z * dotProductWith.z + w * dotProductWith.w);
}

////////////////////////////////
Vec3 Vec4::XYZ() const
{
	return Vec3(x, y, z);
}

const Vec4 operator*(float uniformScale, const Vec4& vecToScale)
{
	return vecToScale * uniformScale;
}
