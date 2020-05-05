#pragma once
#include "Engine/Math/Vec2.hpp"
//-----------------------------------------------------------------------------------------------
struct Vec3
{
public:
	static const Vec3 ZERO;
	static const Vec3 ONE;

	// Construction/Destruction
	~Vec3() {}												// destructor: do nothing (for speed)
	Vec3() {}												// default constructor: do nothing (for speed)
	Vec3(const Vec3& copyFrom);							// copy constructor (from another Vec3)
	explicit Vec3(float initialX, float initialY, float initialZ);		// explicit constructor (from x, y)
	explicit Vec3(const Vec2& extendFrom, float initialZ=0.f);

	/// Format: float, float, float
	void SetFromText(const char* text);
														// Operators
	const Vec3 operator+(const Vec3& vecToAdd) const;			// Vec3 + Vec3
	const Vec3 operator-(const Vec3& vecToSubtract) const;	// Vec3 - Vec3
	const Vec3 operator*(float uniformScale) const;			// Vec3 * float
	const Vec3 operator/(float inverseScale) const;			// Vec3 / float
	const Vec3 operator-() const;
	void operator+=(const Vec3& vecToAdd);					// Vec3 += Vec3
	void operator-=(const Vec3& vecToSubtract);				// Vec3 -= Vec3
	void operator*=(const float uniformScale);				// Vec3 *= float
	void operator/=(const float uniformDivisor);				// Vec3 /= float
	void operator=(const Vec3& copyFrom);						// Vec3 = Vec3
	bool operator==(const Vec3& compare) const;				// Vec3 == Vec3
	bool operator!=(const Vec3& compare) const;				// Vec3 != Vec3

	friend const Vec3 operator*(float uniformScale, const Vec3& vecToScale);	// float * Vec3

	// Getter
	float GetLength() const;
	float GetLengthXY() const;
	float GetLengthSquare() const;
	float GetLengthXYSquare() const;
	float GetAngleDegreesAboutZ() const;
	float GetAngleRadiansAboutZ() const;
	const Vec3 GetRotatedDegreesAboutZ(float degrees) const;
	const Vec3 GetRotatedRadiansAboutZ(float radians) const;
	float DotProduct(const Vec3& dotProductWith) const;
	const Vec3 GetNormalized() const;
	Vec3 CrossProduct(const Vec3& crossProductWith) const;
	Vec2 XY() const { return Vec2(x, y); }

	// Modifier
	void Normalize();
	void SetLength(float length);

public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
};


