#pragma once
#include <vector>
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
//////////////////////////////////////////////////////////////////////////
struct Vertex_PCU;
struct AABB2;
//////////////////////////////////////////////////////////////////////////

void AddVerticesOfDisk2D(
	std::vector<Vertex_PCU>& verts,
	const Vec2& center,
	float radius,
	const Rgba& color,
	int sides = 64
);
void AddVerticesOfLine2D(
	std::vector<Vertex_PCU>& verts,
	const Vec2& start,
	const Vec2& end,
	float thickness,
	const Rgba& color
);
void AddVerticesOfRing2D(
	std::vector<Vertex_PCU>& verts,
	const Vec2& center, float radius,
	float thickness,
	const Rgba& color,
	int sides = 64,
	float z=0.f
);
void AddVerticesOfAABB2D(
	std::vector<Vertex_PCU>& verts,
	const AABB2& box,
	const Rgba& color = Rgba(1.f, 1.f, 1.f),
	const Vec2& bottomLeftTexCoord = Vec2(0.f, 1.f),
	const Vec2& topRightTexCoord = Vec2(1.f, 0.f)
);



Vec2 TransformedPosition(const Vec2& position, float uniformScaleXY, float rotationDegreesAboutZ, const Vec2& translationXY);
Vec3 TransformedPosition(const Vec3& position, float uniformScaleXY, float rotationDegreesAboutZ, const Vec2& translationXY);
void TransformVertex(Vertex_PCU& vertex, float uniformScale, float rotationDegreesAboutZ, const Vec2& translationXY);
void TransformVertexArray(int numVertexes, Vertex_PCU vertices[], float uniformScale, float rotationDegreesAboutZ, const Vec2& translationXY);
void TransformVertexArray(size_t numVertexes, std::vector<Vertex_PCU>& vertices, float uniformScale, float rotationDegreesAboutZ, const Vec2& translationXY);
