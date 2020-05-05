#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Plane2.hpp"
#include "Engine/Math/Capsule2.hpp"
#include <algorithm>
////////////////////////////////
OBB2::OBB2()
{
}

////////////////////////////////
OBB2::OBB2(const Vec2& center, const Vec2& size /*= Vec2::ZERO*/, float rotationDegrees /*= 0.f*/)
	: Center(center)
	, Extends(size * 0.5f)
{
	Right = Vec2(CosDegrees(rotationDegrees), SinDegrees(rotationDegrees));
}

////////////////////////////////
OBB2::OBB2(const AABB2& convertFrom)
{
	Center = convertFrom.GetCenter();
	Extends = convertFrom.GetExtend();
	Right = Vec2(1.f, 0.f);
}

////////////////////////////////
OBB2::OBB2(const Capsule2& convertFrom)
{
	Center = (convertFrom.Start + convertFrom.End) * 0.5f;
	Vec2 disp = convertFrom.End - convertFrom.Start;
	Right = disp.GetNormalized();
	Extends = Vec2(disp.GetLength() * 0.5f, 0.f);
}

////////////////////////////////
OBB2 OBB2::operator+(const Vec2& moveBy) const
{
	OBB2 result(*this);
	result.Center += moveBy;
	return result;
}

////////////////////////////////
void OBB2::Move(const Vec2& moveBy)
{
	Center += moveBy;
}

////////////////////////////////
void OBB2::Rotate(float rotationDegrees)
{
	Right.RotateDegreesAboutOrigin(rotationDegrees);
}

////////////////////////////////
void OBB2::SetPosition(const Vec2& position)
{
	Center = position;
}

////////////////////////////////
void OBB2::SetRotation(float rotationDegrees)
{
	Right = Vec2(CosDegrees(rotationDegrees), SinDegrees(rotationDegrees));
}

////////////////////////////////
Vec2 OBB2::WorldToLocal(const Vec2& worldPosition) const
{
	Vec2 disp = worldPosition - Center;
	float localX = disp.DotProduct(Right);
	float localY = disp.DotProduct(Right.GetRotated90Degrees());
	return Vec2(localX, localY);
}

////////////////////////////////
Vec2 OBB2::LocalToWorld(const Vec2& localPosition) const
{
	return Center + localPosition.x * Right + localPosition.y * Right.GetRotated90Degrees();
}

////////////////////////////////
Vec2 OBB2::GetNearestPoint(const Vec2& worldPoint) const
{
	Vec2 local = WorldToLocal(worldPoint);
	Vec2 clamped = Vec2(
		Clamp(local.x, -Extends.x, Extends.x),
		Clamp(local.y, -Extends.y, Extends.y)
	);
	return LocalToWorld(clamped);
}

///////////////////////////////
bool OBB2::IsIntersectWith(const OBB2& with) const
{
	Plane2 myPlanes[4];		GetPlanes(myPlanes);
	Vec2 myCorners[4];		GetCorners(myCorners);
	Plane2 otherPlanes[4];	with.GetPlanes(otherPlanes);
	Vec2 otherCorners[4];	with.GetCorners(otherCorners);

	for (int planeIdx = 0; planeIdx < 4; ++planeIdx) {
		Plane2& myPlane = myPlanes[planeIdx];
		Plane2& otherPlane = otherPlanes[planeIdx];
		int myFront = 0;
		int otherFront = 0;
		for (int cornerIdx = 0; cornerIdx < 4; ++cornerIdx) {
			Vec2& myCorner = myCorners[cornerIdx];
			Vec2& otherCorner = otherCorners[cornerIdx];

			myFront += (int)myPlane.IsFront(otherCorner);
			otherFront += (int)otherPlane.IsFront(myCorner);
		}
		if (myFront == 4 || otherFront == 4) {
			return false;
		}
	}
	return true;
}

////////////////////////////////
bool OBB2::IsContaining(const Vec2& worldPosition) const
{
	Vec2 local = WorldToLocal(worldPosition);
	Vec2 absLocal = Vec2(fabsf(local.x), fabsf(local.y));
	return absLocal.x <= Extends.x && absLocal.y <= Extends.y;
}

////////////////////////////////
AABB2 OBB2::GetBounding() const
{
	AABB2 result(1e+6f, 1e+6f, -1e+6f, -1e+6f);
	Vec2 myCorners[4];		GetCorners(myCorners);
	for (int i = 0; i < 4; ++i) {
		result.Min.x = std::min(result.Min.x, myCorners[i].x);
		result.Min.y = std::min(result.Min.y, myCorners[i].y);
		result.Max.x = std::max(result.Max.x, myCorners[i].x);
		result.Max.y = std::max(result.Max.y, myCorners[i].y);
	}
	return result;
}

////////////////////////////////
void OBB2::GetPlanes(Plane2* out) const
{
	Vec2 Up = Right.GetRotated90Degrees();
	Vec2 points[] = {
		Center + Right * Extends.x,
		Center + Up * Extends.y,
		Center - Right * Extends.x,
		Center - Up * Extends.y
	};
	out[0] = Plane2(points[0], Right);
	out[1] = Plane2(points[1], Up);
	out[2] = Plane2(points[2], -Right);
	out[3] = Plane2(points[3], -Up);
}

////////////////////////////////
void OBB2::GetCorners(Vec2* out) const
{
	out[0] = GetTopRight();
	out[1] = GetTopLeft();
	out[2] = GetBottomLeft();
	out[3] = GetBottomRight();
}

Vec2 OBB2::GetRight() const
{
	return Right;
}
