#include "Engine/Math/Mat4.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
//////////////////////////////////////////////////////////////////////////
STATIC const Mat4 Mat4::Identity;

//////////////////////////////////////////////////////////////////////////

static inline float __det3x3(const float mat[/*9*/])
{
/*
mat +   -   +
	0	1	2
	3	4	5
	6	7	8
*/
	return
		+ mat[0] * (mat[4] * mat[8] - mat[5] * mat[7])
		- mat[1] * (mat[3] * mat[8] - mat[5] * mat[6])
		+ mat[2] * (mat[3] * mat[7] - mat[4] * mat[6])
		;
}

static inline Mat4 __getInverted(const float mat[/*16*/])
{
/*
mat
	+0	-1	+2	-3
	-4	+5	-6	+7
	+8	-9	+10	-11
	-12	+13	-14	+15
*/
	Mat4 adj;
	const float m0[]  = { mat[5], mat[6], mat[7], mat[9], mat[10], mat[11], mat[13], mat[14], mat[15] };
	const float m1[]  = { mat[4], mat[6], mat[7], mat[8], mat[10], mat[11], mat[12], mat[14], mat[15] };
	const float m2[]  = { mat[4], mat[5], mat[7], mat[8], mat[9] , mat[11], mat[12], mat[13], mat[15] };
	const float m3[]  = { mat[4], mat[5], mat[6], mat[8], mat[9] , mat[10], mat[12], mat[13], mat[14] };
	const float m4[]  = { mat[1], mat[2], mat[3], mat[9], mat[10], mat[11], mat[13], mat[14], mat[15] };
	const float m5[]  = { mat[0], mat[2], mat[3], mat[8], mat[10], mat[11], mat[12], mat[14], mat[15] };
	const float m6[]  = { mat[0], mat[1], mat[3], mat[8], mat[9] , mat[11], mat[12], mat[13], mat[15] };
	const float m7[]  = { mat[0], mat[1], mat[2], mat[8], mat[9] , mat[10], mat[12], mat[13], mat[14] };
	const float m8[]  = { mat[1], mat[2], mat[3], mat[5], mat[6] , mat[7] , mat[13], mat[14], mat[15] };
	const float m9[]  = { mat[0], mat[2], mat[3], mat[4], mat[6] , mat[7] , mat[12], mat[14], mat[15] };
	const float m10[] = { mat[0], mat[1], mat[3], mat[4], mat[5] , mat[7] , mat[12], mat[13], mat[15] };
	const float m11[] = { mat[0], mat[1], mat[2], mat[4], mat[5] , mat[6] , mat[12], mat[13], mat[14] };
	const float m12[] = { mat[1], mat[2], mat[3], mat[5], mat[6] , mat[7] , mat[9] , mat[10], mat[11] };
	const float m13[] = { mat[0], mat[2], mat[3], mat[4], mat[6] , mat[7] , mat[8] , mat[10], mat[11] };
	const float m14[] = { mat[0], mat[1], mat[3], mat[4], mat[5] , mat[7] , mat[8] , mat[9] , mat[11] };
	const float m15[] = { mat[0], mat[1], mat[2], mat[4], mat[5] , mat[6] , mat[8] , mat[9] , mat[10] };

	adj[0]  = +__det3x3(m0);
	adj[1]  = -__det3x3(m1);
	adj[2]  = +__det3x3(m2);
	adj[3]  = -__det3x3(m3);

	adj[4]  = -__det3x3(m4);
	adj[5]  = +__det3x3(m5);
	adj[6]  = -__det3x3(m6);
	adj[7]  = +__det3x3(m7);

	adj[8]  = +__det3x3(m8);
	adj[9]  = -__det3x3(m9);
	adj[10] = +__det3x3(m10);
	adj[11] = -__det3x3(m11);

	adj[12] = -__det3x3(m12);
	adj[13] = +__det3x3(m13);
	adj[14] = -__det3x3(m14);
	adj[15] = +__det3x3(m15);

	float det4x4 = mat[0] * adj[0] +/*-*/ mat[1] * adj[1] + mat[2] * adj[2] +/*-*/ mat[3] * adj[3];
	adj.Transpose();
	adj *= (1.f / det4x4);
	return adj;
}

////////////////////////////////
STATIC Mat4 Mat4::MakeTranslate2D(const Vec2& translateXY)
{
	Mat4 t;
	t[Tx] = translateXY.x;
	t[Ty] = translateXY.y;
	return t;
}

////////////////////////////////
STATIC Mat4 Mat4::MakeUniformScale2D(float scaleXY)
{
	Mat4 s;
	s[Ix] = scaleXY;
	s[Jy] = scaleXY;
	return s;
}

////////////////////////////////
STATIC Mat4 Mat4::MakeRotateDegrees2D(float degrees)
{
	Mat4 r;
	float s = SinDegrees(degrees);
	float c = CosDegrees(degrees);
	r[Ix] = c;
	r[Iy] = s;
	r[Jx] = -s;
	r[Jy] = c;
	return r;
}

////////////////////////////////
STATIC Mat4 Mat4::MakeTranslate3D(const Vec3& translate)
{
	Mat4 t;
	t[Tx] = translate.x;
	t[Ty] = translate.y;
	t[Tz] = translate.z;
	return t;
}

////////////////////////////////
STATIC Mat4 Mat4::MakeUniformScale3D(float scale)
{
	Mat4 s;
	s[Ix] = scale;
	s[Jy] = scale;
	s[Kz] = scale;
	return s;
}

////////////////////////////////
Mat4 Mat4::MakeScale3D(float scaleX, float scaleY, float scaleZ)
{
	Mat4 s;
	s[Ix] = scaleX;
	s[Jy] = scaleY;
	s[Kz] = scaleZ;
	return s;
}

////////////////////////////////
STATIC Mat4 Mat4::MakeRotationXYZ(float eulerDegreeX, float eulerDegreeY, float eulerDegreeZ)
{
	Mat4 rx;
	float c = CosDegrees(eulerDegreeX);
	float s = SinDegrees(eulerDegreeX);
	rx[Jy] = c;
	rx[Jz] = s;
	rx[Ky] = -s;
	rx[Kz] = c;
	Mat4 ry;
	c = CosDegrees(eulerDegreeY);
	s = SinDegrees(eulerDegreeY);
	ry[Ix] = c;
	ry[Iz] = -s;
	ry[Kx] = s;
	ry[Kz] = c;
	Mat4 rz;
	c = CosDegrees(eulerDegreeZ);
	s = SinDegrees(eulerDegreeZ);
	rz[Ix] = c;
	rz[Iy] = s;
	rz[Jx] = -s;
	rz[Jy] = c;
	return rz * ry * rx;
}

////////////////////////////////
Mat4::Mat4()
{
	_ix = 1.f;
	_iy = 0.f;
	_iz = 0.f;
	_iw = 0.f;

	_jx = 0.f;
	_jy = 1.f;
	_jz = 0.f;
	_jw = 0.f;

	_kx = 0.f;
	_ky = 0.f;
	_kz = 1.f;
	_kw = 0.f;

	_tx = 0.f;
	_ty = 0.f;
	_tz = 0.f;
	_tw = 1.f;

}

////////////////////////////////
Mat4::Mat4(const Vec2& iBasis, const Vec2& jBasis, const Vec2& transform/*=Vec2(0, 0)*/)
{
	_ix = iBasis.x;
	_iy = iBasis.y;
	_iz = 0.f;
	_iw = 0.f;

	_jx = jBasis.x;
	_jy = jBasis.y;
	_jz = 0.f;
	_jw = 0.f;

	_kx = 0.f;
	_ky = 0.f;
	_kz = 1.f;
	_kw = 0.f;

	_tx = transform.x;
	_ty = transform.y;
	_tz = 0.f;
	_tw = 1.f;
}

////////////////////////////////
Mat4::Mat4(const Vec3& iBasis, const Vec3& jBasis, const Vec3& kBasis, const Vec3& transform/*=Vec3(0, 0, 0)*/)
{
	_ix = iBasis.x;
	_iy = iBasis.y;
	_iz = iBasis.z;
	_iw = 0.f;

	_jx = jBasis.x;
	_jy = jBasis.y;
	_jz = jBasis.z;
	_jw = 0.f;

	_kx = kBasis.x;
	_ky = kBasis.y;
	_kz = kBasis.z;
	_kw = 0.f;

	_tx = transform.x;
	_ty = transform.y;
	_tz = transform.z;
	_tw = 1.f;
}

////////////////////////////////
Mat4::Mat4(const Vec4& iBasis, const Vec4& jBasis, const Vec4& kBasis, const Vec4& transform)
{
	_ix = iBasis.x;
	_iy = iBasis.y;
	_iz = iBasis.z;
	_iw = iBasis.w;

	_jx = jBasis.x;
	_jy = jBasis.y;
	_jz = jBasis.z;
	_jw = jBasis.w;

	_kx = kBasis.x;
	_ky = kBasis.y;
	_kz = kBasis.z;
	_kw = kBasis.w;

	_tx = transform.x;
	_ty = transform.y;
	_tz = transform.z;
	_tw = transform.w;
}

////////////////////////////////
Mat4::Mat4(const float values[/*16*/])
{
	for (int index = 0; index < 16; ++index)
		m_value[index] = values[index];
}

////////////////////////////////
float& Mat4::operator[](int index)
{
	return m_value[index];
}

////////////////////////////////
const float Mat4::operator[](int index) const
{
	return m_value[index];
}

////////////////////////////////
const Mat4& Mat4::operator=(const Mat4& copyFrom)
{
	for (int index = 0; index < 16; ++index)
		m_value[index] = copyFrom.m_value[index];

	return *this;
}

////////////////////////////////
const Mat4& Mat4::operator*=(float scale)
{
	for (int index = 0; index < 16; ++index) {
		m_value[index] *= scale;
	}
	return *this;
}

////////////////////////////////
const Mat4 Mat4::operator*(const Mat4& rhs) const
{
	Mat4 result;
	result[Ix] = _ix * rhs[Ix] + _jx * rhs[Iy] + _kx * rhs[Iz] + _tx * rhs[Iw];
	result[Iy] = _iy * rhs[Ix] + _jy * rhs[Iy] + _ky * rhs[Iz] + _ty * rhs[Iw];
	result[Iz] = _iz * rhs[Ix] + _jz * rhs[Iy] + _kz * rhs[Iz] + _tz * rhs[Iw];
	result[Iw] = _iw * rhs[Ix] + _jw * rhs[Iy] + _kw * rhs[Iz] + _tw * rhs[Iw];

	result[Jx] = _ix * rhs[Jx] + _jx * rhs[Jy] + _kx * rhs[Jz] + _tx * rhs[Jw];
	result[Jy] = _iy * rhs[Jx] + _jy * rhs[Jy] + _ky * rhs[Jz] + _ty * rhs[Jw];
	result[Jz] = _iz * rhs[Jx] + _jz * rhs[Jy] + _kz * rhs[Jz] + _tz * rhs[Jw];
	result[Jw] = _iw * rhs[Jx] + _jw * rhs[Jy] + _kw * rhs[Jz] + _tw * rhs[Jw];

	result[Kx] = _ix * rhs[Kx] + _jx * rhs[Ky] + _kx * rhs[Kz] + _tx * rhs[Kw];
	result[Ky] = _iy * rhs[Kx] + _jy * rhs[Ky] + _ky * rhs[Kz] + _ty * rhs[Kw];
	result[Kz] = _iz * rhs[Kx] + _jz * rhs[Ky] + _kz * rhs[Kz] + _tz * rhs[Kw];
	result[Kw] = _iw * rhs[Kx] + _jw * rhs[Ky] + _kw * rhs[Kz] + _tw * rhs[Kw];

	result[Tx] = _ix * rhs[Tx] + _jx * rhs[Ty] + _kx * rhs[Tz] + _tx * rhs[Tw];
	result[Ty] = _iy * rhs[Tx] + _jy * rhs[Ty] + _ky * rhs[Tz] + _ty * rhs[Tw];
	result[Tz] = _iz * rhs[Tx] + _jz * rhs[Ty] + _kz * rhs[Tz] + _tz * rhs[Tw];
	result[Tw] = _iw * rhs[Tx] + _jw * rhs[Ty] + _kw * rhs[Tz] + _tw * rhs[Tw];

	return result;
}

////////////////////////////////
const Mat4& Mat4::operator*=(const Mat4& rhs)
{
	return (*this = *this * rhs);
}

////////////////////////////////
const Vec4 Mat4::operator*(const Vec4& rhs) const
{
	Vec4 result;
	result.x = Vec4(_ix, _jx, _kx, _tx).DotProduct(rhs);
	result.y = Vec4(_iy, _jy, _ky, _ty).DotProduct(rhs);
	result.z = Vec4(_iz, _jz, _kz, _tz).DotProduct(rhs);
	result.w = Vec4(_iw, _jw, _kw, _tw).DotProduct(rhs);
	return result;
}

void Mat4::SetI(const Vec4& vec)
{
	I = vec;
}

void Mat4::SetJ(const Vec4& vec)
{
	J = vec;
}

void Mat4::SetK(const Vec4& vec)
{
	K = vec;
}

void Mat4::SetT(const Vec4& vec)
{
	T = vec;
}
////////////////////////////////
Mat4 Mat4::GetTransposed() const
{
	Mat4 transposed(m_value);
	transposed.Transpose();
	return transposed;
}

////////////////////////////////
void Mat4::Transpose()
{
	float m[16];
	for (int index = 0; index < 16; ++index) {
		m[index] = m_value[index];
	}
	m_value[1] = m[4];
	m_value[2] = m[8];
	m_value[3] = m[12];
	m_value[4] = m[1];
	m_value[6] = m[9];
	m_value[7] = m[13];
	m_value[8] = m[2];
	m_value[9] = m[6];
	m_value[11] = m[14];
	m_value[12] = m[3];
	m_value[13] = m[7];
	m_value[14] = m[11];
}

////////////////////////////////
Mat4 Mat4::GetInverted() const
{
	return __getInverted(m_value);
}

////////////////////////////////
Vec2 Mat4::GetTransformedVector2D(const Vec2& vector) const
{
	Vec4 result4 = GetTransformedVec4(Vec4(vector, 0, 0));
	Vec2 result(result4.x, result4.y);
	/*
	result.x = ix * vector.x + jx * vector.y;
	result.y = iy * vector.y + jy * vector.y;
	*/
	return result;
}

////////////////////////////////
Vec2 Mat4::GetTransformedPosition2D(const Vec2& position) const
{
	Vec4 result4 = GetTransformedVec4(Vec4(position, 0, 1));
	Vec2 result(result4.x, result4.y);
// 	result.x = ix * position.x + jx * position.y + tx;
// 	result.y = iy * position.y + jy * position.y + ty;
	return result;
}

////////////////////////////////
Vec3 Mat4::GetTransformedVector3D(const Vec3& vector) const
{
	Vec4 result4 = GetTransformedVec4(Vec4(vector, 0));
	Vec3 result(result4.x, result4.y, result4.z);
// 	result.x = ix * vector.x + jx * vector.y + kx * vector.z;
// 	result.y = iy * vector.y + jy * vector.y + ky * vector.z;
// 	result.z = iz * vector.y + jz * vector.y + kz * vector.z;
	return result;
}

////////////////////////////////
Vec3 Mat4::GetTransformedPosition3D(const Vec3& position) const
{
	Vec4 result4 = GetTransformedVec4(Vec4(position, 1));
	Vec3 result(result4.x, result4.y, result4.z);
// 	result.x = ix * position.x + jx * position.y + kx * position.z + tx;
// 	result.y = iy * position.y + jy * position.y + ky * position.z + ty;
// 	result.z = iz * position.y + jz * position.y + kz * position.z + tz;
	return result;

}

////////////////////////////////
Vec4 Mat4::GetTransformedVec4(const Vec4& v) const
{
	Vec4 result;
	result.x = _ix * v.x + _jx * v.y + _kx * v.z + _tx * v.w;
	result.y = _iy * v.x + _jy * v.y + _ky * v.z + _ty * v.w;
	result.z = _iz * v.x + _jz * v.y + _kz * v.z + _tz * v.w;
	result.w = _iw * v.x + _jw * v.y + _kw * v.z + _tw * v.w;
	return result;
}
