#pragma once

struct Vec3;
struct Vec2;

struct Vec4
{
public:
	float x;
	float y;
	float z;
	float w;

public:
	Vec4() {}
	~Vec4() {}
	Vec4(const Vec4& copyFrom);
	explicit Vec4(float x, float y, float z, float w);
	explicit Vec4(const Vec3& extendFrom, float w=0.f);
	explicit Vec4(const Vec2& extendFrom, float z=0.f, float w=0.f);

	/// Format: float, float, float, float
	void SetFromText(const char* text);

	// Operators
	const Vec4 operator+(const Vec4& vecToAdd) const;			// Vec4 + Vec4
	const Vec4 operator-(const Vec4& vecToSubtract) const;	// Vec4 - Vec4
	const Vec4 operator*(float uniformScale) const;			// Vec4 * float
	const Vec4 operator/(float inverseScale) const;			// Vec4 / float
	const Vec4 operator-() const;
	void operator+=(const Vec4& vecToAdd);					// Vec4 += Vec4
	void operator-=(const Vec4& vecToSubtract);				// Vec4 -= Vec4
	void operator*=(const float uniformScale);				// Vec4 *= float
	void operator/=(const float inverseScale);				// Vec4 /= float
	void operator=(const Vec4& copyFrom);						// Vec4 = Vec4
	bool operator==(const Vec4& compare) const;				// Vec4 == Vec4
	bool operator!=(const Vec4& compare) const;				// Vec4 != Vec4

	float DotProduct(const Vec4& dotProductWith) const;
	Vec3 XYZ() const;
	//float operator *(const Vec4& dotProductWith) const;

	friend const Vec4 operator*(float uniformScale, const Vec4& vecToScale);	// float * Vec4
};