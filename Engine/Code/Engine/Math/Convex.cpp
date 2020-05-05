#include "Engine/Math/Convex.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/RNG.hpp"

ConvexPoly ConvexPoly::GetRandomPoly(float radius)
{
	ConvexPoly result;
	const float start_angle = g_rng.GetFloatInRange(0, 180.f);
	float additional = 0.f;
	const size_t max_points = g_rng.GetIntInRange(3, 12);
	size_t n_points = 0;

	while (n_points <= max_points && additional < 360.f) {
		const float current_angle = start_angle + additional;
		result.m_points.push_back(
			Vec2(
				CosDegrees(current_angle) * radius,
				SinDegrees(current_angle) * radius
			)
		);
		additional += g_rng.GetFloatInRange(5.f, 120.f);
		++n_points;
	}
	
	return result;
}

void ConvexPoly::move_by(const Vec2& disp)
{
	for(auto& each : m_points) {
		each += disp;
	}
}

bool ConvexPoly::is_in_box(const AABB2& box) const
{
	for(auto& eachVert: m_points) {
		if (!box.IsPointInside(eachVert)) {
			return false;
		}
	}
	return true;
}

bool ConvexPoly::is_overlapping_box(const AABB2& box) const
{
		for(auto& eachVert: m_points) {
		if (box.IsPointInside(eachVert)) {
			return true;
		}
	}
	return false;
}

void ConvexPoly::scale(float scale_by, const Vec2& position, const Vec2& scale_center)
{
	const float uniscale = 1.f + scale_by;
	const Vec2 offset = scale_center - position;
	const Vec2 add_back = offset - uniscale * offset;

	move_by(-position);

	for (auto& each: m_points) {
		each *= uniscale;
		each += add_back;
	}

	move_by(position);
}

void ConvexPoly::rotate(float angle, const Vec2& position, const Vec2& center)
{
	//const Vec2 offset = center - position;

	move_by(-center);

	for (auto& each: m_points) {
		//each -= offset;
		each.RotateDegreesAboutOrigin(angle);
		//each += offset;
	}

	move_by(center);
}


ConvexHull2::ConvexHull2(const ConvexPoly& poly)
{
	for(size_t i = 1; i < poly.m_points.size(); ++i) {
		Vec2 normal = poly.m_points[i] - poly.m_points[i-1];
		normal.RotateMinus90Degrees();
		normal.Normalize();
		m_edges.emplace_back(Plane2(poly.m_points[i], normal));
	}
	Vec2 normal = poly.m_points[0] - poly.m_points[ poly.m_points.size() - 1];
	normal.RotateMinus90Degrees();
	normal.Normalize();
	m_edges.emplace_back(Plane2(poly.m_points[0], normal));
}

ConvexImpactResult ConvexHull2::raycast_by(const Ray2& ray)
{
	Plane2* max_hit = nullptr;
	float best_k = -1;
	bool anything_to_hit = false;
	ConvexImpactResult result;
	result.hit = false;
	for(auto& each: m_edges) {
		if(each.IsBehind(ray.start)) {
			continue;
		}
		anything_to_hit = true;
		float k = ray.RaycastToPlane2(each);
		if (k > 0.f) {
			if (k > best_k) {
				max_hit = &each;
				best_k = k;
			}
		}
	}

	if (!anything_to_hit) {
		result.hit = true;
		result.pos = ray.start;
		result.normal = -ray.dir;
		result.k = 0;
		return result;
	}
	if (!max_hit) {
		return result;
	}
	
	Vec2 p = ray.GetPointAt(best_k);
	if (is_inside(p, max_hit)) {
		result.hit = true;
		result.pos = p;
		result.k = best_k;
		result.normal = max_hit->Normal;
	}
	return result;
}

bool ConvexHull2::is_inside(const Vec2& p, Plane2* ignore)
{
	for(auto& each : m_edges) {
		if(&each == ignore) {
			continue;
		}
		if (each.IsFront(p)) {
			return false;
		}
	}
	return true;
}