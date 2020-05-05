#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
//////////////////////////////////////////////////////////////////////////
///
/// This Mat4 class is supposed to be worked under MSVC complier (CL.EXE)
///
//////////////////////////////////////////////////////////////////////////
enum Mat4Index
{
	/*
	Ix, Jx, Kx, Tx,
	Iy, Jy, Ky, Ty,
	Iz, Jz, Kz, Tz,
	Iw, Jw, Kw, Tw,
	*/
	Ix, Iy, Iz, Iw, Jx, Jy, Jz, Jw,
	Kx, Ky, Kz, Kw, Tx, Ty, Tz, Tw,
};
//////////////////////////////////////////////////////////////////////////
struct Mat4
{
public:
	static const Mat4 Identity;
	union
	{
		float m_value[16];
#pragma warning(push)
#pragma warning(disable:4201)
		// C4201: nonstandard extension used : nameless struct/union
		// Under Microsoft extensions (/Ze),
		// you can specify a structure without a declarator as members of another structure or union.
		// These structures generate an error under ANSI compatibility (/Za).
		/// These aliases can ONLY be used in Mat4.cpp
		struct
		{
			/*
			float _ix; float _jx; float _kx; float _tx;
			float _iy; float _jy; float _ky; float _ty;
			float _iz; float _jz; float _kz; float _tz;
			float _iw; float _jw; float _kw; float _tw;
			*/
			float _ix; float _iy; float _iz; float _iw;
			float _jx; float _jy; float _jz; float _jw;
			float _kx; float _ky; float _kz; float _kw;
			float _tx; float _ty; float _tz; float _tw;

		};
		struct
		{
			Vec4 I;
			Vec4 J;
			Vec4 K;
			Vec4 T;
		};
#pragma warning(pop)
	};
public:
	static Mat4 MakeTranslate2D(const Vec2& translateXY);
	static Mat4 MakeUniformScale2D(float scaleXY);
	static Mat4 MakeRotateDegrees2D(float degrees);
	
	static Mat4 MakeTranslate3D(const Vec3& translate);
	static Mat4 MakeUniformScale3D(float scale);
	static Mat4 MakeScale3D(float scaleX, float scaleY, float scaleZ);
	static Mat4 MakeRotationXYZ(float eulerDegreeX, float eulerDegreeY, float eulerDegreeZ);

	Mat4();
	Mat4(const Mat4& copyFrom) { *this = copyFrom; }
	~Mat4() { /*DO NOTHING*/};
	explicit Mat4(const Vec2& iBasis, const Vec2& jBasis, const Vec2& transform=Vec2(0, 0));
	explicit Mat4(const Vec3& iBasis, const Vec3& jBasis, const Vec3& kBasis, const Vec3& transform=Vec3(0, 0, 0));
	explicit Mat4(const Vec4& iBasis, const Vec4& jBasis, const Vec4& kBasis, const Vec4& transform);
	explicit Mat4(const float values[/*16*/]);
	float& operator[](int index);
	const float operator[](int index) const;
	
	const Mat4& operator=(const Mat4& copyFrom);
	const Mat4 operator*(const Mat4& rhs) const;
	const Mat4& operator*=(const Mat4& rhs);
	const Mat4& operator*=(float scale);
	const Vec4 operator*(const Vec4& rhs) const;

	Vec4 GetI() const { return Vec4(_ix, _iy, _iz, _iw); }
	Vec4 GetJ() const { return Vec4(_jx, _jy, _jz, _jw); }
	Vec4 GetK() const { return Vec4(_kx, _ky, _kz, _kw); }
	Vec4 GetT() const { return Vec4(_tx, _ty, _tz, _tw); }

	void SetI(const Vec4& vec);
	void SetJ(const Vec4& vec);
	void SetK(const Vec4& vec);
	void SetT(const Vec4& vec);


	//float GetRank() const;
	Mat4 GetTransposed() const;
	void Transpose();
	Mat4 GetInverted() const;
	//void Invert();

	//////////////////////////////////////////////////////////////////////////
	//
	//[Ix Jx Kx Tx]   [x]
	//[Iy Jy Ky Ty] * [y]
	//[Iz Jz Kz Tz]   [z]
	//[Iw Jw Kw Tx]   [w]
	//    Mat4       Homo3D
	//
	//////////////////////////////////////////////////////////////////////////
	Vec2 GetTransformedVector2D(const Vec2& vector) const;
	Vec2 GetTransformedPosition2D(const Vec2& position) const;
	Vec3 GetTransformedVector3D(const Vec3& vector) const;
	Vec3 GetTransformedPosition3D(const Vec3& position) const;
	Vec4 GetTransformedVec4(const Vec4& v) const;

};
