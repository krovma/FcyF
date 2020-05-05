#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include <math.h>

#define M_EPSILONF 1e-6f
//////////////////////////////////////////////////////////////////////////
STATIC const Vec3 Vec3::ZERO(0.f, 0.f, 0.f);
STATIC const Vec3 Vec3::ONE(1.f, 1.f, 1.f);


//-----------------------------------------------------------------------------------------------
Vec3::Vec3(const Vec3& copy)
	: x(copy.x)
	, y(copy.y)
	, z(copy.z)
{
}


//-----------------------------------------------------------------------------------------------
Vec3::Vec3(float initialX, float initialY, float initialZ)
	: x(initialX)
	, y(initialY)
	, z(initialZ)
{
}


Vec3::Vec3(const Vec2& extendFrom, float initialZ/*=0.f*/)
	: x(extendFrom.x)
	, y(extendFrom.y)
	, z(initialZ)
{
}

////////////////////////////////
void Vec3::SetFromText(const char* text)
{
	std::vector<std::string> splited = Split(text, ',');
	if (splited.size() != 3) {
		ERROR_AND_DIE(Stringf("[ERROR]%s: Cannot convert %s to Vec3", FUNCTION, text));
		return;
	}
	x = (float)std::atof(splited[0].c_str());
	y = (float)std::atof(splited[1].c_str());
	z = (float)std::atof(splited[2].c_str());
}

//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator + (const Vec3& vecToAdd) const
{
	return Vec3(x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z);
}


//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator-(const Vec3& vecToSubtract) const
{
	return Vec3(x - vecToSubtract.x, y - vecToSubtract.y, z-vecToSubtract.z);

}

////////////////////////////////
const Vec3 Vec3::operator-() const
{
	return Vec3(-x, -y, -z);
}

//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator*(float uniformScale) const
{
	return Vec3(uniformScale * x, uniformScale * y, uniformScale * z);
}

//-----------------------------------------------------------------------------------------------
float Vec3::GetLength() const
{
	return sqrtf(x * x + y * y + z * z);
}

float Vec3::GetLengthXY() const
{
	return sqrtf(x * x + y * y);
}

float Vec3::GetLengthSquare() const
{
	return x * x + y * y + z * z;
}

float Vec3::GetLengthXYSquare() const
{
	return x * x + y * y;
}

float Vec3::GetAngleDegreesAboutZ() const
{
	return Atan2Degrees(y, x);
}

float Vec3::GetAngleRadiansAboutZ() const
{
	return atan2f(y, x);
}

const Vec3 Vec3::GetRotatedRadiansAboutZ(float radians) const
{
	float c = cos(radians);
	float s = sin(radians);
	float newX = c * x - s * y;
	float newY = s * x + c * y;
	return Vec3(newX, newY, z);
}

////////////////////////////////
float Vec3::DotProduct(const Vec3& dotProductWith) const
{
	return x * dotProductWith.x + y * dotProductWith.y + z * dotProductWith.z;
}

////////////////////////////////
const Vec3 Vec3::GetNormalized() const
{
	Vec3 result(*this);
	result.SetLength(1.f);
	return result;
}

////////////////////////////////
Vec3 Vec3::CrossProduct(const Vec3& crossProductWith) const
{
	return Vec3(
		y * crossProductWith.z - z * crossProductWith.y,
		z  *crossProductWith.x - x * crossProductWith.z,
		x * crossProductWith.y - y * crossProductWith.x
	);
}

void Vec3::Normalize()
{
	SetLength(1.f);
}

////////////////////////////////
void Vec3::SetLength(float length)
{
	float oldLength = GetLength();
	if (FloatEq(oldLength, 0.f)) {
		x = length;
		y = 0.f;
		z = 0.f;
	} else {
		float scale = length / oldLength;
		y *= scale;
		x *= scale;
		z *= scale;
	}
}

const Vec3 Vec3::GetRotatedDegreesAboutZ(float degrees) const
{
	return GetRotatedRadiansAboutZ(ConvertDegreesToRadians(degrees));
}

//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator/(float inverseScale) const
{
	return Vec3(x / inverseScale, y / inverseScale, z / inverseScale);
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator+=(const Vec3& vecToAdd)
{
	x += vecToAdd.x;
	y += vecToAdd.y;
	z += vecToAdd.z;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator-=(const Vec3& vecToSubtract)
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	z -= vecToSubtract.z;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator*=(const float uniformScale)
{
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator/=(const float uniformDivisor)
{
	x /= uniformDivisor;
	y /= uniformDivisor;
	z /= uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator=(const Vec3& copyFrom)
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
}


//-----------------------------------------------------------------------------------------------
const Vec3 operator*(float uniformScale, const Vec3& vecToScale)
{
	return vecToScale * uniformScale;
}


//-----------------------------------------------------------------------------------------------
bool Vec3::operator==(const Vec3& compare) const
{
	return
		(	x == compare.x
		&&	y == compare.y
		&&	z == compare.z
		);
}


//-----------------------------------------------------------------------------------------------
bool Vec3::operator!=(const Vec3& compare) const
{
	return !(*this == compare);
}

