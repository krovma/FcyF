#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Capsule3.hpp"
#include "Engine/Math/MathUtils.hpp"
////////////////////////////////
Capsule3::Capsule3(const Vec3& center, float radius /*= 0.f*/)
	: Start(center)
	, End(center)
	, Radius(radius)
{
}

////////////////////////////////
Capsule3::Capsule3(const Vec3& start, const Vec3& end, float radius /*= 0.f*/)
	: Start(start)
	, End(end)
	, Radius(radius)
{

}

////////////////////////////////
Capsule3 Capsule3::operator+(const Vec3& moveBy) const
{
	return Capsule3(Start + moveBy, End + moveBy, Radius);
}

////////////////////////////////
void Capsule3::Move(const Vec3& moveBy)
{
	Start += moveBy;
	End += moveBy;
}

////////////////////////////////
void Capsule3::SetPosition(const Vec3& position)
{
	Vec3 disp = (End - Start) * 0.5f;
	Start = position - disp;
	End = position + disp;
}

////////////////////////////////
bool Capsule3::IsContaining(const Vec3& position) const
{
	const Vec3 np = GetNearestPointOnSegment3(position, Start, End);
	const float d2 = GetDistanceSquare(np, position);
	return (d2 < Radius * Radius)
	|| FloatEq(d2, Radius * Radius, 1e-5f);
}

////////////////////////////////
Vec3 Capsule3::GetNearestPoint(const Vec3& position) const
{
	if (IsContaining(position)) {
		return position;
	}
	Vec3 np = GetNearestPointOnSegment3(position, Start, End);
	Vec3 disp = position - np;

	disp.SetLength(Radius);
	return np + disp;
}

