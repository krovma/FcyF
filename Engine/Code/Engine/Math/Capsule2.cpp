#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Capsule2.hpp"
#include "Engine/Math/MathUtils.hpp"
////////////////////////////////
Capsule2::Capsule2(const Vec2& center, float radius /*= 0.f*/)
	: Start(center)
	, End(center)
	, Radius(radius)
{
}

////////////////////////////////
Capsule2::Capsule2(const Vec2& start, const Vec2& end, float radius /*= 0.f*/)
	: Start(start)
	, End(end)
	, Radius(radius)
{

}

////////////////////////////////
Capsule2 Capsule2::operator+(const Vec2& moveBy) const
{
	return Capsule2(Start + moveBy, End + moveBy, Radius);
}

////////////////////////////////
void Capsule2::Move(const Vec2& moveBy)
{
	Start += moveBy;
	End += moveBy;
}

////////////////////////////////
void Capsule2::Rotate(float rotationDegrees)
{
	Vec2 center = (Start + End) * 0.5f;
	Vec2 disp = (End - Start) * 0.5f;
	disp.RotateDegreesAboutOrigin(rotationDegrees);
	Start = center - disp;
	End = center + disp;
}

////////////////////////////////
void Capsule2::SetPosition(const Vec2& position)
{
	Vec2 disp = (End - Start) * 0.5f;
	Start = position - disp;
	End = position + disp;
}

////////////////////////////////
void Capsule2::SetRotation(float rotationDegrees)
{
	Vec2 disp = End - Start;
	disp.SetAngleDegrees(rotationDegrees);
	End = Start + disp;
}

////////////////////////////////
bool Capsule2::IsContaining(const Vec2& position) const
{
	Vec2 np = GetNearestPointOnSegment2(position, Start, End);
	if ((np - position).GetLengthSquare() <= Radius * Radius) {
		return true;
	} else {
		return false;
	}
}

////////////////////////////////
Vec2 Capsule2::GetNearestPoint(const Vec2& position) const
{
	if (IsContaining(position)) {
		return position;
	}
	Vec2 np = GetNearestPointOnSegment2(position, Start, End);
	Vec2 disp = position - np;

	disp.SetLength(Radius);
	return np + disp;
}

