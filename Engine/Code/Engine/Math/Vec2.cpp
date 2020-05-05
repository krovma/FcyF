#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <math.h>
#include <vector>
#include <string>

#define M_EPSILONF 1e-6f
//////////////////////////////////////////////////////////////////////////
STATIC const Vec2 Vec2::ZERO(0.f, 0.f);
STATIC const Vec2 Vec2::ONE(1.f, 1.f);
//////////////////////////////////////////////////////////////////////////
Vec2 Vec2::MakeFromPolarDegrees(float thetaDegrees, float length /*= 1.f*/)
{
	return Vec2(length * CosDegrees(thetaDegrees), length * SinDegrees(thetaDegrees));
}

//////////////////////////////////////////////////////////////////////////
Vec2 Vec2::MakeFromPolarRadians(float thetaRadians, float length /*= 1.f*/)
{
	return Vec2(length * cosf(thetaRadians), length * sinf(thetaRadians));
}

//-----------------------------------------------------------------------------------------------
Vec2::Vec2( const Vec2& copy )
	: x( copy.x )
	, y( copy.y )
{
}


//-----------------------------------------------------------------------------------------------
Vec2::Vec2( float initialX, float initialY )
	: x( initialX )
	, y( initialY )
{
}


////////////////////////////////
Vec2::Vec2(const IntVec2& convertFrom)
	: x((float)convertFrom.x)
	, y((float)convertFrom.y)
{

}

////////////////////////////////
void Vec2::SetFromText(const char* text)
{
	std::vector<std::string> splited = Split(text,',');
	if (splited.size() != 2) {
		ERROR_AND_DIE(Stringf("[ERROR]%s: Cannot convert %s to Vec2", FUNCTION, text));
		return;
	}
	x = (float) std::atof(splited[0].c_str());
	y = (float) std::atof(splited[1].c_str());
}

void Vec2::set_from_buffer_reader(buffer_reader& reader)
{
	x = reader.next_basic<float>();
	y = reader.next_basic<float>();
}

size_t Vec2::append_to_buffer_writer(buffer_writer& writer) const
{
	writer.append_multi_byte(x);
	writer.append_multi_byte(y);
	return 2 * sizeof(float);
}

//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator + ( const Vec2& vecToAdd ) const
{
	return Vec2( x + vecToAdd.x, y + vecToAdd.y );
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator-(const Vec2& vecToSubtract) const
{
	return Vec2(x - vecToSubtract.x, y - vecToSubtract.y);

}

////////////////////////////////
const Vec2 Vec2::operator-() const
{
	return Vec2(-x, -y);
}

////////////////////////////////
float Vec2::DotProduct(const Vec2& dotProductWith) const
{
	return (x * dotProductWith.x) + (y * dotProductWith.y);
}

////////////////////////////////
float Vec2::CrossProduct(const Vec2& crossProductWith) const
{
	return x * crossProductWith.y - y * crossProductWith.x;
}

//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator*( float uniformScale ) const
{
	return Vec2(uniformScale * x, uniformScale * y);
}

const Vec2 Vec2::operator*(const Vec2& mul) const
{
	return Vec2(mul.x * x, mul.y * y);
}

//-----------------------------------------------------------------------------------------------
float Vec2::GetLength() const
{
	return sqrtf(x * x + y * y);
}

float Vec2::GetLengthSquare() const
{
	return x * x + y * y;
}

float Vec2::GetAngleDegrees() const
{
	return Atan2Degrees(y, x);
}

float Vec2::GetAngleRadians() const
{
	return atan2f(y, x);
}

const Vec2 Vec2::GetRotated90Degrees() const
{
	return Vec2(-y, x);
}

const Vec2 Vec2::GetRotatedMinus90Degrees() const
{
	return Vec2(y, -x);
}

const Vec2 Vec2::GetClamped(float maxLength) const
{
	Vec2 clamped(x, y);
	clamped.ClampLength(maxLength);
	return clamped;
}

const Vec2 Vec2::GetNormalized() const
{
	Vec2 normalized(x, y);
	normalized.Normalize();
	return normalized;
}

void Vec2::SetLength(float length)
{
	float oldLength = GetLength();
	if (FloatEq(oldLength, 0.f)) {
		x = length;
		y = 0.f;
	}
	else {
		float scale = length / oldLength;
		y *= scale;
		x *= scale;
	}
}

void Vec2::ClampLength(float maxLength)
{
	if (FloatGt(GetLength(), maxLength)) {
		SetLength(maxLength);
	}
}

void Vec2::SetAngleDegrees(float angleDegrees)
{
	SetAngleRadians(ConvertDegreesToRadians(angleDegrees));
}

void Vec2::SetAngleRadians(float angleRadians)
{
	Vec2 newVec = MakeFromPolarRadians(angleRadians, GetLength());
	x = newVec.x;
	y = newVec.y;
}

void Vec2::SetPolarDegrees(float angleDegrees, float length)
{
	SetPolarRadians(ConvertDegreesToRadians(angleDegrees), length);
}

void Vec2::SetPolarRadians(float angleRadians, float length)
{
	Vec2 maked = MakeFromPolarRadians(angleRadians, length);
	x = maked.x;
	y = maked.y;
}

void Vec2::RotateDegreesAboutOrigin(float degrees)
{
	RotateRadiansAboutOrigin(ConvertDegreesToRadians(degrees));
}

void Vec2::RotateRadiansAboutOrigin(float radians)
{
/*
	float r = GetLength();
	float rotated_th = atan2f(y, x) + radians;
	x = r * cosf(rotated_th);
	y = r * sinf(rotated_th);
*/
	float c = cos(radians);
	float s = sin(radians);
	float newX = c * x - s * y;
	float newY = s * x + c * y;
	x = newX;
	y = newY;
}

void Vec2::Rotate90Degrees()
{
	float t = x;
	x = -y;
	y = t;
}

void Vec2::RotateMinus90Degrees()
{
	float t = x;
	x = y;
	y = -t;
}

void Vec2::Normalize()
{
	SetLength(1.f);
}

float Vec2::GetLengthThenNormalize()
{
	float length = GetLength();
	SetLength(1.f);
	return length;
}

const Vec2 Vec2::GetRotatedDegreesAboutOrigin(float degrees) const
{
	return GetRotatedRadiansAboutOrigin(ConvertDegreesToRadians(degrees));
}

const Vec2 Vec2::GetRotatedRadiansAboutOrigin(float radians) const	//A not so elegant way
{
	float c = cos(radians);
	float s = sin(radians);
	float newX = c * x - s * y;
	float newY = s * x + c * y;
	return Vec2(newX, newY);
}

////////////////////////////////
bool Vec2::AllComponentLessThan(const Vec2& vecToCompare) const
{
	return (x < vecToCompare.x && y < vecToCompare.y);
}

////////////////////////////////
bool Vec2::AllComponentGreatThan(const Vec2& vecToCompare) const
{
	return (x > vecToCompare.x && y > vecToCompare.y);
}

//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator/( float inverseScale ) const
{
	return Vec2(x / inverseScale, y / inverseScale);
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator+=( const Vec2& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator-=( const Vec2& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator*=( const float uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator/=( const float uniformDivisor )
{
	x /= uniformDivisor;
	y /= uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator=( const Vec2& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
}


//-----------------------------------------------------------------------------------------------
const Vec2 operator*( float uniformScale, const Vec2& vecToScale )
{
	return vecToScale * uniformScale;
}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator==( const Vec2& compare ) const
{
	return (x == compare.x && y == compare.y);
}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator!=( const Vec2& compare ) const
{
	return !(*this == compare);
}

