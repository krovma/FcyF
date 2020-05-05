#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Plane2.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Ray.hpp"
#include "Engine/Math/AABB2.hpp"

struct ConvexImpactResult
{
	bool hit = false;
	Vec2 pos;
	Vec2 normal;
	float k=1e6;
};

class ConvexPoly
{
public:
	static ConvexPoly GetRandomPoly(float radius = 1.f);
	void move_by(const Vec2& disp);
	bool is_in_box(const AABB2& box) const;
	bool is_overlapping_box(const AABB2& box) const;

	void scale(float scale_by, const Vec2& position, const Vec2& scale_center=Vec2::ZERO);
	void rotate(float angle, const Vec2& position, const Vec2& center=Vec2::ZERO);
public:
	std::vector<Vec2> m_points;
	
};

class ConvexHull2
{
public:
	std::vector<Plane2> m_edges;

	ConvexHull2(const ConvexPoly& poly);
	ConvexHull2() = default;
	ConvexImpactResult raycast_by(const Ray2& ray);
	bool is_inside(const Vec2& p, Plane2* ignore=nullptr);
	void move_by(const Vec2& disp);
};