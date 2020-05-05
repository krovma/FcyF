#pragma once

template<typename Type>
constexpr decltype(auto) Clamp(const Type& value, const Type& min, const Type& max);

template<typename Type>
inline constexpr int Sgn(const Type& value);

template<typename Type, typename PositionType>
constexpr decltype(auto) Lerp(const Type& a, const Type& b, PositionType pos);

float ConvertRadiansToDegrees(float radians);
float ConvertDegreesToRadians(float degrees);
float SinDegrees(float degrees);
float CosDegrees(float degrees);
float TanDegrees(float degrees);
float Atan2Degrees(float y, float x);

bool FloatEq(float lhs, float rhs, float epsilon = 1e-6f);
bool FloatGt(float lhs, float rhs, float epsilon = 1e-6f);
bool FloatLt(float lhs, float rhs, float epsilon = 1e-6f);

int Quadratic(float* roots, float a, float b, float c);

float FloatMap(float input, float inputMin, float inputMax, float outputMin, float outputMax); //Linear Map for Now

struct Vec2;
struct Vec3;
struct AABB2;
struct Mat4;

float GetDistance(const Vec2& positionA, const Vec2& positionB);
float GetDistance(const Vec3& positionA, const Vec3& positionB);
float GetDistanceXY(const Vec3& positionA, const Vec3& positionB);
float GetDistanceSquare(const Vec2& positionA, const Vec2& positionB);
float GetDistanceSquare(const Vec3& positionA, const Vec3& positionB);
float GetDistanceXYSquare(const Vec3& positionA, const Vec3& positionB);

float GetAngleDisplacementDegrees(float startDegrees, float endDegrees);
float GetTurnedAngleDegrees(float currentDegrees, float goalDegrees, float maxDeltaDegrees);

Mat4 GetRotationXYZFromAToB(const Vec3& a, const Vec3& b);
Vec3 GlobalToLocal(const Vec3& global, const Vec3& i, const Vec3& j, const Vec3& k);

bool DoDiskOverlap(const Vec2& centerA, float radiusA, const Vec2& centerB, float radiusB);
bool DoSphereOverlap(const Vec3& centerA, float radiusA, const Vec3& centerB, float radiusB);

struct Vertex_PCU;
//#ToDo: Safely remove these legacy code
Vec2 TransformedPosition_Legacy(const Vec2& position, float uniformScaleXY, float rotationDegreesAboutZ, const Vec2& translationXY);
Vec3 TransformedPosition_Legacy(const Vec3& position, float uniformScaleXY, float rotationDegreesAboutZ, const Vec2& translationXY);
void TransformVertex_Legacy(Vertex_PCU& vertex, float uniformScale, float rotationDegreesAboutZ, const Vec2& translationXY);
void TransformVertexArray_Legacy(int numVertexes, Vertex_PCU vertexes[], float uniformScale, float rotationDegreesAboutZ, const Vec2& translationXY);

Vec2 GetNearestPointOnAABB2(const Vec2& point, const AABB2& box);
Vec2 GetNearestPointOnSegment2(const Vec2& point, const Vec2& start, const Vec2& end);
Vec3 GetNearestPointOnSegment3(const Vec3& point, const Vec3& start, const Vec3& end);
Vec2 GetProjectedPointOnSegment2(const Vec2& point, const Vec2& start, const Vec2& end);
bool DoDiskAABB2Overlap(const Vec2& center, float radius, const AABB2& box);
Vec2 PushDiskOutFromAABB2(const Vec2& center, float radius, const AABB2& box);
Vec2 PushDiskOutFromDisk(const Vec2& centerPush, float radiusPush, const Vec2& centerFrom, float radiusFrom);
Vec2 GetReflectedByDisk(const Vec2& incident, const Vec2& center, const Vec2& hitPointOnDisk, float bounce = 1.f, float smoothness = 1.f);

bool IsPointInTriangle(const Vec2& point, const Vec2& a, const Vec2& b, const Vec2& c);

/// param: x already clamped to 0, 1
float SmoothStep3(float x);
float SmoothStep5(float x);
float SmoothStart2(float x);
float SmoothStart3(float x);
float SmoothStart4(float x);
float SmoothStart5(float x);
float SmoothEnd2(float x);
float SmoothEnd3(float x);
float SmoothEnd4(float x);
float SmoothEnd5(float x);


//////////////////////////////////////////////////////////////////////////
// template implements
//////////////////////////////////////////////////////////////////////////
template<typename Type>
constexpr decltype(auto) Clamp(const Type& value, const Type& min, const Type& max)
{
	return value < min ? min : (value > max ? max : value);
}

////////////////////////////////
template<typename Type>
constexpr int Sgn(const Type& value)
{
	return (int)(Type(0) < value) - (int)(value < Type(0));
}

////////////////////////////////
template<typename Type, typename PositionType>
constexpr decltype(auto) Lerp(const Type& a, const Type& b, PositionType pos)
{
	return (((PositionType)1.f - pos) * a) + (pos * b);
}

