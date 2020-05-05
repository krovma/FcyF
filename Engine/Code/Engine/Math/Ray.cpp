#include "Engine/Math/Ray.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Plane3.hpp"
#include "Engine/Math/Plane2.hpp"
#include "Engine/Math/Capsule3.hpp"
#include "Engine/Math/AABB2.hpp"

#include <algorithm>
#include "Engine/Core/ErrorWarningAssert.hpp"

Ray3::Ray3(const Vec3& start, const Vec3& dir)
	: start(start)
	, dir(dir.GetNormalized())
{
}

Ray3 Ray3::FromPoint(const Vec3& start, const Vec3& pointOnRay)
{
	Vec3 dir = pointOnRay - start;
	return Ray3(start, dir);
}

Vec3 Ray3::GetPointAt(float k) const
{
	return start + k * dir;
}

Vec3 Ray3::GetClampedPointAt(float k) const
{
	return GetPointAt(k < 0 ? 0 : k);
}

float Ray3::RaycastToCapsule3(const Capsule3& capsule) const
{
	// 1. check if the ray collide with the inf cylinder
	float cylinderK = RaycastToInfCylinder(capsule.Start, capsule.End - capsule.Start, capsule.Radius);
	if (cylinderK < 0) {
		// the ray must intersect with the inf cylinder
		return -1.f;
	}
	// 2. check if the point in step 1 is on the capsule
	Vec3 cylinderPoint = GetPointAt(cylinderK);
	if (capsule.IsContaining(cylinderPoint)) {
		return cylinderK;
	}
	// 3. if the point in step 1 is not on the capsule, check the sphere
	float sphere1k = RaycastToSphere(capsule.Start, capsule.Radius);
	float sphere2k = RaycastToSphere(capsule.End, capsule.Radius);
	if (sphere1k > sphere2k) {
		std::swap(sphere1k, sphere2k);
	}
	if (sphere2k < 0) {
		return -1.f;
	}
	if (sphere1k < 0) {
		return sphere2k;
	}
	return sphere1k;
}

float Ray3::RaycastToPlane3(const Plane3& plane) const
{
	const float distanceFromOrigin = -plane.Distance;
	const float toGo = distanceFromOrigin - start.DotProduct(plane.Normal);
	const float result = toGo / dir.DotProduct(plane.Normal);
	return result;
}

Ray2::Ray2(const Vec2& start, const Vec2& dir)
	: start(start)
	, dir(dir.GetNormalized())
{
}

Ray2 Ray2::FromPoint(const Vec2& start, const Vec2& pointOnRay)
{
	Vec2 dir = pointOnRay - start;
	return Ray2(start, dir);
}

Vec2 Ray2::GetPointAt(float k) const
{
	return start + k * dir;
}

Vec2 Ray2::GetClampedPointAt(float k) const
{
	return GetPointAt(k < 0 ? 0 : k);
}

float Ray2::RaycastToPlane2(const Plane2& plane) const
{
	const float distanceFromOrigin = -plane.Distance;
	const float toGo = distanceFromOrigin - start.DotProduct(plane.Normal);
	const float result = toGo / dir.DotProduct(plane.Normal);
	return result;
}

float Ray2::RaycastToAABB2(const AABB2& box) const
{
	if (box.IsPointInside(start)) {
		return 0.f;
	}
	const float x_speed = 1.f / dir.x;
	const float y_speed = 1.f / dir.y;

	const float tx1 = (box.Min.x - start.x) * x_speed;
	const float tx2 = (box.Max.x - start.x) * x_speed;
	const float ty1 = (box.Min.y - start.y) * y_speed;
	const float ty2 = (box.Max.y - start.y) * y_speed;
 
	const float max_of_min = std::max(std::min(tx1, tx2), std::min(ty1, ty2));
	const float min_of_max = std::min(std::max(tx1, tx2), std::max(ty1, ty2));

	float result;
	if (min_of_max < 0) {
	    return -1;
	}

	if (max_of_min > min_of_max) {
	    return -1;
	}

	return max_of_min;
}


float Ray3::RaycastToInfCylinder(const Vec3& center, const Vec3& cdir, float radius) const
{
	Vec3 n = cdir.GetNormalized();
	if (fabsf(n.DotProduct(dir)) == 1.f) {
		Vec3 cp = start - center;
		Vec3 nearest = center + n * (cp.DotProduct(n));
		if (GetDistanceSquare(nearest, start) == radius * radius) {
			return 0.f;
		}
	}
	Vec3 a = dir.DotProduct(n) * n - dir;
	Vec3 b = center + start.DotProduct(n) * n - center.DotProduct(n) * n - start;
	float A = a.DotProduct(a);
	float B = a.DotProduct(b) * 2.f;
	float C = b.DotProduct(b) - radius * radius;
	float root[2] = { 0, 0 };
	int nRoots = Quadratic(root, A, B, C);
	if (nRoots == 0) {
		return -1.f;
	}
	if (nRoots == 2) {
		// ReSharper disable CppUnreachableCode
		if (root[0] > root[1]) {
			std::swap(root[0], root[1]);
		}
		if (root[1] < 0.f) {
			return -1.f;
		}
		if (root[0] < 0.f) {
			return root[1];
		}
		return root[0];
	}
	ERROR_AND_DIE("IMPOSSIBLE MATH ERROR");
}

float Ray3::RaycastToSphere(const Vec3& center, float radius) const
{
	Vec3 A = start - center;
	constexpr float a = 1.f;
	const float b = 2.f * A.DotProduct(dir);
	const float c = A.GetLengthSquare() - radius * radius;
	float root[2] = { 0.f, 0.f };
	int nRoots = Quadratic(root, a, b, c);
	if (nRoots == 0) {
		return -1.f;
	}
	if (nRoots == 1) {
		if (root[0] < 0.f) {
			return -1.f;
		}
		return root[0];
	}
	if (nRoots == 2) {
		if (root[0] > root[1]) {
			std::swap(root[0], root[1]);
		}
		if (root[1] < 0.f) {
			return -1.f;
		}
		if (root[0] < 0.f) {
			return root[1];
		}
		return root[0];
	}
	ERROR_AND_DIE("IMMPOSIBLE MATH ERROR");
}
